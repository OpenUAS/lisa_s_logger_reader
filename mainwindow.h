#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort>
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QFileDialog>


namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;


  QSerialPort *serial;
  QByteArray data;
  QStandardItemModel *model;

private slots:

  void readData();
  void on_pushButton_clicked();
  void update_table();
  void on_export_button_clicked();
};

#endif // MAINWINDOW_H
