#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort>
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QTimer>

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

private slots:

  void readData();
  void fetch_memory();
  void update_table();
  void on_export_button_clicked();
  void updateData();
};

#endif // MAINWINDOW_H
