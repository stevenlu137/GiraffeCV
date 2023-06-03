#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

	private slots:
	void on_pushButtonSnapShot_clicked();

	void on_pushButtonPlay_clicked();

	void on_toolButton_clicked();

	void on_toolButton_2_clicked();

	void on_comboBoxPanoTypeList_currentTextChanged(const QString &arg1);

	void on_pushButtonAddURL_clicked();

	void on_pushButtonClear_clicked();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
