#include "mainwindow.h"
#include "ui_mainwindow.h"


//#define DATA_SIZE 2


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  serial = new QSerialPort(this);
  connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

  fetch_button = new QPushButtonProgress(this, "Dump memory");
  ui->button_spot->addWidget(fetch_button);
  connect(fetch_button, SIGNAL(clicked()), this, SLOT(fetch_memory()));

  model = new QStandardItemModel(0,0,this);
  ui->table->setModel(model);

  newDataRead = 0;

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateData()));
  timer->start(1000);

  QTimer *timer2 = new QTimer(this);
  connect(timer2, SIGNAL(timeout()), this, SLOT(updateRaw()));
  timer2->start(1000);

  decodeProgressBar = new QProgressBar();
  decodeData = new QLabel();
  decodeData->setText("Decoded data");
  ui->horizontalLayoutDecodeProgress->addWidget(decodeData);
  ui->horizontalLayoutDecodeProgress->addWidget(decodeProgressBar);
  decodeProgressBar->hide();


  serialPortList = new QComboBox(this);
  listAvailablePorts = QSerialPortInfo::availablePorts();
  serialPortList->addItem("");
  for(int i=0; i<listAvailablePorts.size(); i++) serialPortList->addItem(listAvailablePorts.at(i).portName());
  serialPortList->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
  ui->button_spot->addWidget(serialPortList);
  connect(serialPortList, SIGNAL(currentIndexChanged(int)), this, SLOT(configureSerialConnexion()));
}

MainWindow::~MainWindow()
{
  delete ui;
}



void MainWindow::configureSerialConnexion(){

  serial->close();

  int currentIndex = serialPortList->currentIndex();
  if(currentIndex < 1){ //if we selected the blank value we quit

      return;
  }

  serial->setPort(listAvailablePorts.at(currentIndex-1)); //-1 cause of the first blank value in the list
  serial->setBaudRate(115200);    //115200/230400/921600
  serial->setDataBits(QSerialPort::Data8);
  serial->setParity(QSerialPort::NoParity);
  serial->setStopBits(QSerialPort::OneStop);
  serial->setFlowControl(QSerialPort::NoFlowControl);

  if(!serial->open(QIODevice::ReadWrite)){

      qDebug("error when opening the serial port");
      QMessageBox::critical(this, tr("Error"), serial->errorString());
  }
}

void MainWindow::readData()
{
  data.append(serial->readAll());

  newDataRead = 1;

  if(!serial->putChar('B')){

      qDebug("error when sending over the serial port");
      QMessageBox::critical(this, tr("Error"), serial->errorString());
  }

  timer->start(); //act as a restart on the timer
}


void MainWindow::updateRaw(){

  ui->lcdNumber->display(data.size());

  fetch_button->setProgress(data.size()*100/33554432);    //33554432 = 32MB, the total size of the memory
}



void MainWindow::updateData(){

  if(newDataRead){

    ui->data->setText(QString(data.toHex()));
    update_table();
  }

  newDataRead = 0;
  fetch_button->hideProgressBar();
}



void MainWindow::fetch_memory()
{

  if(!serial->putChar('A')){

      qDebug("error when sending over the serial port");
      QMessageBox::critical(this, tr("Error"), serial->errorString());
  }

  ui->data->clear();
  this->data.clear();

  fetch_button->showProgressBar();
}



void MainWindow::update_table()
{
  char start_log_sequence[6] = {0xAA, 0x55, 0xFF, 0x00, 0x55, 0xAA};
  char start_values_sequence[3] = {0xF0, 0xF0, 0xA5};

  char start_lost_values_sequence[6] = {0x42, 0x0F, 0X42, 0X00, 0XFF, 0xAA};
  char stop_lost_values_sequence[6] = {0xAA, 0xFF, 0x00, 0x42, 0x0F, 0x42};

  char stop_log_sequence[6] = {0xFF, 0x00, 0x55, 0xAA, 0x00, 0xFF};
  int start_of_log, start_of_values, end_of_log;
  int nbr_messages;
  QList<QByteArray> messages_names;



  //decode the name of the messages
  start_of_log = data.indexOf(QByteArray(start_log_sequence, 6));
  start_of_values = data.indexOf(QByteArray(start_values_sequence, 3));

  unsigned char data_size = data.mid(start_of_log+6, 1).at(0);



  messages_names = data.mid(start_of_log+7, start_of_values-6).split(';');    //DAFUK size ???
  nbr_messages = messages_names.size();


  model->clear();
  model = new QStandardItemModel(0, nbr_messages, this);
  for(int i=0; i<nbr_messages; i++){

      model->setHorizontalHeaderItem(i, new QStandardItem(QString(messages_names.at(i))));
  }

  ui->table->setModel(model);





  //fill in the lost values by zeros
  while(data.contains(start_lost_values_sequence)){

    unsigned long start, stop;
    long nbr_lost = 0;

    start = data.indexOf(QByteArray(start_lost_values_sequence, 6));
    stop = data.indexOf(QByteArray(stop_lost_values_sequence, 6));


    for(int k=0; k<data_size; k++){

      nbr_lost += (static_cast<quint8>(data.mid(start+6, 4).at(k)))<<(8*k);   //we get 32bits value every times
    }

    QByteArray lost_data_insert;

    for(int i=0; i<nbr_lost; i++){

      lost_data_insert.append((char) 0x00);
    }

    data.remove(start, stop-start+6);
    data.insert(start, lost_data_insert);
  }





  //decode the values
  end_of_log = data.indexOf(QByteArray(stop_log_sequence, 6));
  if(end_of_log < 0) return;


  decodeProgressBar->show();
  decodeProgressBar->setValue(0);
  qApp->processEvents();    //we force qt to process the events to display the progress bar


  QByteArray values = data.mid(start_of_values+3, end_of_log-(start_of_values+3));

  for(int i=0; i< (values.size()/(nbr_messages*data_size)); i++){ //for each pack of values (each row of the table)

    QByteArray subValues = values.mid(i*data_size*nbr_messages, nbr_messages*data_size);
    QList<QStandardItem *> items;

    for(int j=0; j<nbr_messages; j++){  //for each messages

        QByteArray currentValue = subValues.mid(j*data_size, data_size);
        long currentLongValue = 0;

        if(currentValue.size() < data_size) break;

        for(int k=0; k<data_size; k++){

          currentLongValue += static_cast<quint8>(currentValue.at(k))<<(8*k);
        }

        long lastBit = currentLongValue & (0x01<<((8*data_size)-1));

        if(lastBit){  //if lastBit != 0 : we need to add ones to the begining of currentLongValue

          for(int k=data_size; k<4; k++){ //fill with the last bit the remaining of the long

            currentLongValue = currentLongValue | (0xFF<<(8*k));
          }
        }


        QStandardItem *item = new QStandardItem(QString::number(currentLongValue));
        items.append(item);
    }

    model->appendRow(items);

    int progress = (i*100)/(values.size()/(nbr_messages*data_size));
    if(progress != decodeProgressBar->value()){

      ui->lcd_lines->display(model->rowCount());
      decodeProgressBar->setValue(i*100/(values.size()/(nbr_messages*data_size)));
      qApp->processEvents();    //we force qt to process the events to display the progress bar
    }
  }

  ui->lcd_lines->display(model->rowCount());
  decodeProgressBar->hide();
}



void MainWindow::on_export_button_clicked()
{
  QString filename = QFileDialog::getSaveFileName(this, "Export data", "", "*.csv");
  if(filename.isEmpty()) return;
  if (!filename.endsWith(".csv", Qt::CaseInsensitive) ) filename += ".csv";

  QFile f(filename);
  if(!f.open(QIODevice::WriteOnly)){

      //error when opening the file
      return;
  }
  QTextStream file(&f);


  file << "Time;";

  for(int i=0; i<model->columnCount(); i++){

    file << model->horizontalHeaderItem(i)->text().toLatin1() << ";";
  }

  file << "\n\r";

  for(int i=0; i<model->rowCount(); i++){

    file << QString::number(i).toLatin1() << ";";

    for(int j=0; j<model->columnCount(); j++){

      file << model->item(i, j)->text().toLatin1() << ";";
    }

    file << "\n\r";
  }

  f.close();
}


void MainWindow::on_actionDump_memory_triggered()
{
  fetch_memory();
}


void MainWindow::on_actionClear_interface_triggered()
{
  data.clear();
  ui->lcdNumber->display(0);
  ui->data->setText("");
  ui->lcd_lines->display(0);
  model->clear();
}

void MainWindow::on_actionExport_data_triggered()
{
  on_export_button_clicked();
}



