#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <regex>
#include <filesystem>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->listVideoSourceURLs->setDragDropMode(QAbstractItemView::InternalMove);
	ui->listVideoSourceURLs->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButtonSnapShot_clicked()
{
	ui->panoRender->SnapShot();
}

void MainWindow::on_pushButtonPlay_clicked()
{
	ui->panoRender->Release();
	ui->comboBoxPanoTypeList->clear();


	ui->panoRender->SetProfilePath(ui->lineEditProfile->text());

	std::vector<QString> vItems;
	for(int i = 0; i < ui->listVideoSourceURLs->count(); ++i)
	{
		QListWidgetItem* item = ui->listVideoSourceURLs->item(i);
		vItems.push_back(item->text());
	}

	ui->panoRender->OpenVideos(vItems);

	std::vector<QString> vPanoTypeList = ui->panoRender->GetPanoTypeList();
	for (auto e : vPanoTypeList)
	{
		ui->comboBoxPanoTypeList->addItem(e);
	}

}

void MainWindow::on_comboBoxPanoTypeList_currentTextChanged(const QString &arg1)
{
    ui->panoRender->SetPanoType(arg1);
}

void MainWindow::on_toolButton_clicked()
{
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(tr("Open profile"));
	fileDialog->setDirectory(".");
	if (fileDialog->exec() == QDialog::Accepted)
	{
		QString path = fileDialog->selectedFiles()[0];
		ui->lineEditProfile->setText(path);
	}
}

void MainWindow::on_toolButton_2_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, "Select video path", "."
		, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	ui->lineEditVideoPath->setText(path);
}

void MainWindow::on_pushButtonAddURL_clicked()
{
	std::string sURLStr = ui->lineEditVideoPath->text().toStdString();
	std::istringstream issIPs(sURLStr);
	std::string sURL = "";
	while (issIPs >> sURL)
	{
		ui->listVideoSourceURLs->addItem(QString::fromUtf8(sURL.c_str()));
	}
}

void MainWindow::on_pushButtonClear_clicked()
{
    ui->listVideoSourceURLs->clear();
}

