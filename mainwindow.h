#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QTimer>
#include <QProgressBar>
#include <QLabel>
#include <QApplication>
#include <QComboBox>

#include "qpushbuttonprogress.h"

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
  QPushButtonProgress *fetch_button;
  int newDataRead;
  QTimer *timer;

  QProgressBar *decodeProgressBar;
  QLabel *decodeData;
  QComboBox *serialPortList;
  QList<QSerialPortInfo> listAvailablePorts;

private slots:

  void readData();
  void fetch_memory();
  void update_table();
  void on_export_button_clicked();
  void updateData();
  void updateRaw();
  void on_actionDump_memory_triggered();
  void on_actionClear_interface_triggered();
  void on_actionExport_data_triggered();
  void configureSerialConnexion();
};

#endif // MAINWINDOW_H
