#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QCheckBox>
#include <QLineEdit>
#include <QDateEdit>

namespace Ui {
class MainWindow;
}

class QSqlTableModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTimer *timer;
    void setModelOnTable(QString);

public slots:
    void changeImage();

private slots:
    void on_ingresarBtn_clicked();
    void on_cerrarSesionBtn_clicked();
    void on_cobrarMenuBtn_clicked();
    void on_miembrosMenuBtn_clicked();
    void on_citasMenuBtn_clicked();

    void on_btnCrearMiembro_clicked();
    void on_btnEditarMiembro_clicked();
    void on_btnEliminarMiembro_clicked();

    void on_tableMiembro_clicked(const QModelIndex &index);
    void on_btnDeseleccionar_clicked();

    void on_serviciosMenuBtn_clicked();

    void on_empleadosMenuBtn_clicked();

    void on_searchBtn_clicked();

    void on_searchBtnDel_clicked();

    void on_pushButton_clicked();

    void on_cancelButton_clicked();

    void on_hidePopUp_clicked();

    void hideMemberFields(int);

    void clearLayout();

    void on_addButton_clicked();

    void on_editButton_clicked();

    void on_citasPorFecha_clicked();


    void on_agendarBtn_clicked();

    void on_citasTable_clicked(const QModelIndex &index);

    void on_eliminarCita_clicked();

    void on_tableCitasCobro_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QSqlTableModel *model;
    QSqlDatabase db;
    QString currentTable;

    // Clientes_Miembros
    QCheckBox *esMiembro;
    QLineEdit *nombreIn;
    QLineEdit *telefonoIn;
    QDateEdit *fechaIn;
    QLineEdit *puntosIn;


    // Servicio
    QLineEdit *precioIn;
    QLineEdit *descripcionIn;
    QLineEdit *nombreServIn;

    // Empleado
    QLineEdit *rolIn;
    QTimeEdit *entradaIn;
    QTimeEdit *salidaIn;
    QLineEdit *salarioIn;
    QLineEdit *nombreEmpIn;
};

#endif // MAINWINDOW_H
