#include "mainwindow.h"
#include "ui_mainwindow.h"


#define DATA_SIZE 4


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  serial = new QSerialPort(this);
  serial->setPortName("/dev/ttyACM1");
  serial->setBaudRate(115200);
  serial->setDataBits(QSerialPort::Data8);
  serial->setParity(QSerialPort::NoParity);
  serial->setStopBits(QSerialPort::OneStop);
  serial->setFlowControl(QSerialPort::NoFlowControl);

  if(!serial->open(QIODevice::ReadWrite)){

      qDebug("error when opening the serial port");
      QMessageBox::critical(this, tr("Error"), serial->errorString());
  }




  model = new QStandardItemModel(2,3,this);
  ui->table->setModel(model);





  connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::readData()
{
  data.append(serial->readAll());

  ui->data->setText(QString(data.toHex()));

  update_table();

  ui->lcdNumber->display(data.size());
}




void MainWindow::on_pushButton_clicked()
{
  QByteArray data = "A";
  serial->putChar('A');

  ui->data->clear();
  this->data.clear();
}



void MainWindow::update_table()
{
  char start_log_sequence[6] = {0xAA, 0x55, 0xFF, 0x00, 0x55, 0xAA};
  char start_values_sequence[3] = {0xF0, 0xF0, 0xA5};
  char stop_log_sequence[6] = {0xFF, 0x00, 0x55, 0xAA, 0x00, 0xFF};
  int start_of_log, start_of_values, end_of_log;
  int nbr_messages;
  QList<QByteArray> messages_names;



  //decode the name of the messages
  start_of_log = data.indexOf(QByteArray(start_log_sequence, 6));
  start_of_values = data.indexOf(QByteArray(start_values_sequence, 3));

  messages_names = data.mid(start_of_log+6, start_of_values-6).split(';');
  nbr_messages = messages_names.size();


  model->clear();
  model = new QStandardItemModel(0, nbr_messages, this);
  for(int i=0; i<nbr_messages; i++){

      model->setHorizontalHeaderItem(i, new QStandardItem(QString(messages_names.at(i))));
  }

  ui->table->setModel(model);



  //decode the values
  end_of_log = data.indexOf(QByteArray(stop_log_sequence, 6));

  QByteArray values = data.mid(start_of_values+3, end_of_log-(start_of_values+3));

  for(int i=0; i< (values.size()/nbr_messages); i++){ //for each pack of values (each row of the table)

    QByteArray subValues = values.mid(i*DATA_SIZE*nbr_messages, nbr_messages*DATA_SIZE);
    QList<QStandardItem *> items;

    for(int j=0; j<nbr_messages; j++){  //for each messages

        QByteArray currentValue = subValues.mid(j*DATA_SIZE, DATA_SIZE);
        double currentDoubleValue = 0;

        if(currentValue.size() < DATA_SIZE) break;

        for(int k=0; k<DATA_SIZE; k++){

          currentDoubleValue += currentValue.at(k)<<(8*k);
        }


        QStandardItem *item = new QStandardItem(QString::number(currentDoubleValue));
        items.append(item);

    }

    model->appendRow(items);
  }
}





