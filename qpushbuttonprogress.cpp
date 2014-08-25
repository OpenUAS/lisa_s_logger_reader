#include "qpushbuttonprogress.h"

QPushButtonProgress::QPushButtonProgress(QWidget *parent, QString text) :
  QPushButton(parent)
{

  this->text = text;

  QHBoxLayout *layout = new QHBoxLayout(this);


  progressBar = new QProgressBar(this);
  progressBar->setAlignment(Qt::AlignHCenter);
  progressBar->setTextVisible(false);

  layout->addWidget(progressBar);

  progressBarIsHidden = 1;
  setProgress(0);
  update();
}


void QPushButtonProgress::showProgressBar(void){

  progressBarIsHidden = 0;
  update();
}

void QPushButtonProgress::hideProgressBar(void){

  progressBarIsHidden = 1;
  update();
}


void QPushButtonProgress::setProgress(int advancement){

  if(advancement>100) progressBar->setValue(100);
  else progressBar->setValue(advancement);
}


void QPushButtonProgress::update(){

  if(progressBarIsHidden){

    this->setText(this->text);
    progressBar->hide();
    this->setEnabled(true);
  }else{

    this->setText("");
    progressBar->show();
    this->setEnabled(false);
  }
}






