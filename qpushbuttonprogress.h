#ifndef QPUSHBUTTONPROGRESS_H
#define QPUSHBUTTONPROGRESS_H

#include <QPushButton>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QString>

class QPushButtonProgress : public QPushButton
{
  Q_OBJECT
public:
  explicit QPushButtonProgress(QWidget *parent = 0, QString text = "");

signals:

public slots:
  void showProgressBar(void);
  void hideProgressBar(void);
  void setProgress(int advancement);


private:
  int progressBarIsHidden;
  QString text;
  QProgressBar *progressBar;

private slots:
  void update();


};

#endif // QPUSHBUTTONPROGRESS_H
