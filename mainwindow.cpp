#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QtCore>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QTimeEdit>
#include <QDateEdit>
#include <QCheckBox>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidgetMain->setCurrentIndex(1);
    setWindowIcon(QIcon(":/assets/assets/icon.png"));

    // SETUP LOGIN IMAGES  ------------------------------------------------------------
    // Login Image Slider
    timer = new QTimer;
    timer->setSingleShot(true);
    changeImage();
    connect(timer, SIGNAL(timeout()), this, SLOT(changeImage()));
    timer->start(5000);

    // STYLES              ------------------------------------------------------------
    // logo image and QLinEdit glow
    QPixmap logo(":/assets/assets/logo.png");
    ui->logoLbl->setPixmap(logo.scaled(100,100, Qt::KeepAspectRatio));
    ui->logoMenuImg->setPixmap(logo.scaled(50,50, Qt::KeepAspectRatio));
    ui->usuarioLed->setAttribute(Qt::WA_MacShowFocusRect,0);
    ui->contrasenaLed->setAttribute(Qt::WA_MacShowFocusRect,0);


    ui->errorLoginLbl->setVisible(false);
    ui->btnEditarMiembro->setVisible(false);
    ui->btnEliminarMiembro->setVisible(false);
    ui->btnDeseleccionar->setVisible(false);
    ui->popup->setVisible(false);
    ui->searchBtnDel->setVisible(false);
    ui->eliminarCita->setVisible(false);


    // SETUP DATABASE     ------------------------------------------------------------
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("moncayo_barberia");
    db.setUserName("root");
    db.setPassword("password");

    if(db.open()){
        qDebug() << "connected " << db.hostName() << db.databaseName();
    } else {
        qDebug() << "Connection FAILED.";
    }


    on_miembrosMenuBtn_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setModelOnTable(QString table)
{
    // Table
    model = new QSqlTableModel(this);
    model->setTable(table);
    model->select();
    const QModelIndex indexE = model->index(0, 0);
    model->setData(indexE, Qt::AlignCenter, Qt::TextAlignmentRole);

    ui->tableMiembro->setModel(model);

    // Combo box
    QSqlQueryModel *comboModel = new QSqlQueryModel;
    QSqlQuery filtroQry;
    filtroQry.prepare("DESCRIBE " + table);
    filtroQry.exec();

    comboModel->setQuery(filtroQry);
    ui->filtro->setModel(comboModel);

    on_btnDeseleccionar_clicked();
}

void MainWindow::on_ingresarBtn_clicked()
{
    QString username = ui->usuarioLed->text();
    QString password = ui->contrasenaLed->text();

    QSqlQuery authQry;
    authQry.prepare("SELECT u.usuario, u.contraseña, u.esAdmin, e.nombre FROM usuario AS u INNER JOIN empleado AS e ON u.idempleado = e.idempleado WHERE usuario = '"+ username +"' AND contraseña = '"+ password +"'");

    if(authQry.exec()){
        if(authQry.next()){
            ui->stackedWidgetMain->setCurrentIndex(0);
            ui->nombreLbl->setText(authQry.value(3).toString());

            if(authQry.value(2) == 1){
                ui->cobrarMenuBtn->setVisible(false);
                ui->citasMenuBtn->setVisible(false);
                ui->miembrosMenuBtn->setVisible(true);
                ui->empleadosMenuBtn->setVisible(true);
                ui->serviciosMenuBtn->setVisible(true);
            } else{
                ui->cobrarMenuBtn->setVisible(true);
                ui->citasMenuBtn->setVisible(true);
                ui->miembrosMenuBtn->setVisible(true);
                ui->empleadosMenuBtn->setVisible(false);
                ui->serviciosMenuBtn->setVisible(false);
            }

            ui->errorLoginLbl->setVisible(false);

        } else{
            ui->errorLoginLbl->setVisible(true);
        }
    } else{
        qDebug() << "ERROR con el Query de autenticación";
    }

}

void MainWindow::on_cerrarSesionBtn_clicked()
{
    ui->stackedWidgetMain->setCurrentIndex(1);
}

void MainWindow::on_cobrarMenuBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->miembrosMenuBtn->setChecked(false);
    ui->citasMenuBtn->setChecked(false);
    ui->serviciosMenuBtn->setChecked(false);
    ui->empleadosMenuBtn->setChecked(false);
    on_searchBtnDel_clicked();

    QSqlQueryModel *citasModel = new QSqlQueryModel;
    QSqlQuery citasQry;
    citasQry.prepare("SELECT c.idcita, c.fecha, c.hora, e.nombre AS `Empleado`, cl.nombre AS `Cliente`, COUNT(spc.idcita) AS `# serv agendados` FROM cita AS c INNER JOIN empleado AS e ON c.idempleado = e.idempleado INNER JOIN cliente AS cl ON c.idcliente = cl.idcliente LEFT JOIN servicioporcita AS spc ON c.idcita = spc.idcita GROUP BY c.idcita ORDER BY c.hora");
    citasQry.exec();

    citasModel->setQuery(citasQry);
    ui->tableCitasCobro->setModel(citasModel);

}

void MainWindow::on_citasMenuBtn_clicked()
{
    ui->fechaCitaIn->setDate(QDate::currentDate());
    ui->fechaTabla->setDate(QDate::currentDate());

    ui->stackedWidget->setCurrentIndex(2);
    ui->miembrosMenuBtn->setChecked(false);
    ui->cobrarMenuBtn->setChecked(false);
    ui->serviciosMenuBtn->setChecked(false);
    ui->empleadosMenuBtn->setChecked(false);
    on_searchBtnDel_clicked();

    QSqlQueryModel *clientesModel = new QSqlQueryModel;
    QSqlQueryModel *empleadosModel = new QSqlQueryModel;
    QSqlQueryModel *serviciosModel = new QSqlQueryModel;
    QSqlQuery clientesQry, empleadosQry, serviciosQry;

    clientesQry.prepare("SELECT nombre FROM cliente");
    empleadosQry.prepare("SELECT nombre FROM empleado");
    serviciosQry.prepare("SELECT idservicio, nombre, precio, descripcion FROM servicio");
    clientesQry.exec();
    empleadosQry.exec();
    serviciosQry.exec();
    clientesModel->setQuery(clientesQry);
    empleadosModel->setQuery(empleadosQry);
    serviciosModel->setQuery(serviciosQry);

    ui->clienteCitaIn->setModel(clientesModel);
    ui->empleadoCitaIn->setModel(empleadosModel);
    ui->serviciosTableIn->setModel(serviciosModel);


}



void  MainWindow::clearLayout() {
    for(int i = ui->popupLayout->rowCount(); i >=0; i--)
    ui->popupLayout->removeRow(i);
}

void MainWindow::on_miembrosMenuBtn_clicked()
{
    currentTable = "clientes_miembros";
    ui->stackedWidget->setCurrentIndex(1);
    ui->cobrarMenuBtn->setChecked(false);
    ui->citasMenuBtn->setChecked(false);
    ui->serviciosMenuBtn->setChecked(false);
    ui->empleadosMenuBtn->setChecked(false);
    on_searchBtnDel_clicked();
    ui->gestionarLbl->setText("Gestionar Clientes y Miembros");

    setModelOnTable("clientes_miembros");

    clearLayout();

    nombreIn = new QLineEdit;
    telefonoIn = new QLineEdit;
    fechaIn = new QDateEdit;
    fechaIn->setCalendarPopup(true);
    puntosIn = new QLineEdit;
    esMiembro = new QCheckBox;
    connect(esMiembro, SIGNAL(stateChanged(int)), this, SLOT(hideMemberFields(int)));

    ui->popupLayout->addRow("Nombre: ", nombreIn);
    ui->popupLayout->addRow("Telefono: ", telefonoIn);
    ui->popupLayout->addRow("Es miembro ", esMiembro);
    ui->popupLayout->addRow("Fecha de inicio de membresía: ", fechaIn);
    ui->popupLayout->addRow("Puntos acumulados: ", puntosIn);

    hideMemberFields(0);
}

void MainWindow::hideMemberFields(int checkState){

  int rowCount = ui->popupLayout->count()/2;

  if (esMiembro->isChecked()) {
    ui->popupLayout->itemAt(rowCount-1,QFormLayout::FieldRole)->widget()->show();
    ui->popupLayout->itemAt(rowCount-2,QFormLayout::FieldRole)->widget()->show();
    ui->popupLayout->itemAt(rowCount-1,QFormLayout::LabelRole)->widget()->show();
    ui->popupLayout->itemAt(rowCount-2,QFormLayout::LabelRole)->widget()->show();
  }
  else {
    ui->popupLayout->itemAt(rowCount-1,QFormLayout::FieldRole)->widget()->hide();
    ui->popupLayout->itemAt(rowCount-2,QFormLayout::FieldRole)->widget()->hide();
    ui->popupLayout->itemAt(rowCount-1,QFormLayout::LabelRole)->widget()->hide();
    ui->popupLayout->itemAt(rowCount-2,QFormLayout::LabelRole)->widget()->hide();
  }
}

void MainWindow::on_serviciosMenuBtn_clicked()
{
    currentTable = "servicio";
    ui->stackedWidget->setCurrentIndex(1);
    ui->cobrarMenuBtn->setChecked(false);
    ui->citasMenuBtn->setChecked(false);
    ui->miembrosMenuBtn->setChecked(false);
    ui->empleadosMenuBtn->setChecked(false);
    on_searchBtnDel_clicked();
    ui->gestionarLbl->setText("Gestionar Servicios");

    setModelOnTable("servicio");

    clearLayout();
    precioIn = new QLineEdit;
    precioIn->setValidator(new QDoubleValidator(0,999,2,this));
    descripcionIn = new QLineEdit;
    nombreServIn = new QLineEdit;
    ui->popupLayout->addRow("Precio ($MXN): ", precioIn);
    ui->popupLayout->addRow("Descripción: ", descripcionIn);
    ui->popupLayout->addRow("Nombre: ", nombreServIn);
}

void MainWindow::on_empleadosMenuBtn_clicked()
{
    currentTable = "empleado";
    ui->stackedWidget->setCurrentIndex(1);
    ui->cobrarMenuBtn->setChecked(false);
    ui->citasMenuBtn->setChecked(false);
    ui->serviciosMenuBtn->setChecked(false);
    ui->miembrosMenuBtn->setChecked(false);
    on_searchBtnDel_clicked();
    ui->gestionarLbl->setText("Gestionar Empleados");

    setModelOnTable("empleado");

    clearLayout();
    rolIn = new QLineEdit;
    entradaIn = new QTimeEdit;
    salidaIn = new QTimeEdit;
    salarioIn = new QLineEdit;
    salarioIn->setValidator(new QDoubleValidator(0,99999,2,this));
    nombreEmpIn = new QLineEdit;
    ui->popupLayout->addRow("Rol: ", rolIn);
    ui->popupLayout->addRow("Hora de entrada: ", entradaIn);
    ui->popupLayout->addRow("Hora de salida: ", salidaIn);
    ui->popupLayout->addRow("Salario: ", salarioIn);
    ui->popupLayout->addRow("Nombre: ", nombreEmpIn);

}




void MainWindow::on_searchBtn_clicked()
{
    // Combo box
    QString filtro = ui->filtro->currentText();
    QString filtroTxt = ui->searchBar->text();

    QSqlQueryModel *filtroModel = new QSqlQueryModel;
    QSqlQuery filtroQry;
    filtroQry.prepare("SELECT * FROM " + currentTable + " WHERE " + filtro + " LIKE \"%" + filtroTxt + "%\" ");
    filtroQry.exec();


    filtroModel->setQuery(filtroQry);
    ui->tableMiembro->setModel(filtroModel);

    ui->searchBtnDel->setVisible(true);

    ui->tableMiembro->setStyleSheet("border: 1px solid #C2A062");
}

void MainWindow::on_searchBtnDel_clicked()
{
    ui->searchBar->setText("");
    ui->filtro->setCurrentText("");
    ui->searchBtn->setVisible(true);
    ui->searchBtnDel->setVisible(false);
    setModelOnTable(currentTable);

    ui->tableMiembro->setStyleSheet("border: 1px solid rgba(0,0,0,0.2)");
}


void MainWindow::on_pushButton_clicked()
{
    ui->popup->setVisible(false);
}

void MainWindow::on_cancelButton_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Cancelar operación", "¿Está seguro de que desea cancelar la operación?",
                                QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ui->popup->setVisible(false);
    }
}


void MainWindow::on_hidePopUp_clicked()
{
    ui->popup->setVisible(false);
}






void MainWindow::on_addButton_clicked()
{
    QSqlQuery addItemQry, addSecond;

    if(currentTable == "clientes_miembros"){
        addItemQry.prepare("INSERT INTO cliente (nombre, telefono) VALUES ('"+ nombreIn->text() + "', '"+ telefonoIn->text() +"')");

        if(!addItemQry.exec())
            QMessageBox::critical(this, "ERROR ", addItemQry.lastError().text());
        else{
            if(esMiembro->isChecked()){
                addSecond.prepare("INSERT INTO miembro (`fecha inicio`, puntos, idCliente) VALUES ('" + fechaIn->date().toString("yy-MM-dd") + "', "+ puntosIn->text() +", ( SELECT MAX(idcliente) FROM cliente))");
                if(!addSecond.exec())
                    QMessageBox::critical(this, "ERROR ", addSecond.lastError().text());
            }
        }
    }
    else if(currentTable == "servicio"){
        addItemQry.prepare("INSERT INTO servicio (precio, descripcion, nombre) VALUES ( '"+ precioIn->text() +"', '"+ descripcionIn->text() +"', '"+ nombreServIn->text() +"')");
        if(!addItemQry.exec())
            QMessageBox::critical(this, "ERROR ", addItemQry.lastError().text());
    }
    else if(currentTable == "empleado"){
        addItemQry.prepare("INSERT INTO empleado (rol, horarioEntrada, horarioSalida, salario, nombre) VALUES ('"+ rolIn->text() +"', '"+ entradaIn->text() +"', '"+ salidaIn->text() +"', '"+ salarioIn->text() +"', '"+ nombreEmpIn->text() +"')");
        if(!addItemQry.exec())
            QMessageBox::critical(this, "ERROR ", addItemQry.lastError().text());
    }

    on_hidePopUp_clicked();
    model->select();
}

void MainWindow::on_editButton_clicked()
{

    QSqlQuery miembroQry, editQry;

    int selectedRow = ui->tableMiembro->selectionModel()->selectedRows()[0].row();
    QString idAEditar = ui->tableMiembro->model()->index(selectedRow, 0).data().toString();

    if(currentTable == "clientes_miembros"){

        bool eraMiembro = ui->tableMiembro->model()->index(selectedRow, 3).data().toString() != "0";
        bool esMiembroAhora = esMiembro->isChecked();
        QString idMiembro = ui->tableMiembro->model()->index(selectedRow, 3).data().toString();

        if(eraMiembro && esMiembroAhora){
            miembroQry.prepare("UPDATE miembro SET `fecha inicio` = '"+ fechaIn->text() +"', puntos = '"+ puntosIn->text() +"' WHERE (idcliente = '"+ idAEditar +"');");
            if(!miembroQry.exec())
                QMessageBox::critical(this, "ERROR ", miembroQry.lastError().text());
        } else if(eraMiembro && !esMiembroAhora){
            miembroQry.prepare("DELETE FROM miembro WHERE (idmiembro = '"+ idMiembro +"')");
            if(!miembroQry.exec())
                QMessageBox::critical(this, "ERROR ", miembroQry.lastError().text());
        } else if(!eraMiembro && esMiembroAhora){
            miembroQry.prepare("INSERT INTO miembro (`fecha inicio`, puntos, idcliente) VALUES ('"+ fechaIn->text() +"', '"+ puntosIn->text() +"', '"+ idAEditar +"')");
            if(!miembroQry.exec())
                QMessageBox::critical(this, "ERROR ", miembroQry.lastError().text());
        }

        editQry.prepare("UPDATE cliente SET nombre = '"+ nombreIn->text() +"', telefono = '"+ telefonoIn->text() +"' WHERE (`idcliente` = '"+ idAEditar +"')");
        if(!editQry.exec())
            QMessageBox::critical(this, "ERROR ", editQry.lastError().text());
    }
    else if(currentTable == "servicio"){
        editQry.prepare("UPDATE servicio SET precio = '"+ precioIn->text() +"', descripcion = '"+ descripcionIn->text() +"', nombre = '"+ nombreServIn->text() +"' WHERE (`idservicio` = '"+ idAEditar +"')");
        if(!editQry.exec())
            QMessageBox::critical(this, "ERROR ", editQry.lastError().text());
    }
    else if(currentTable == "empleado"){
        editQry.prepare("UPDATE empleado SET rol = '"+ rolIn->text() +"', horarioEntrada = '"+ entradaIn->text() +"', `horarioSalida` = '"+ salidaIn->text() +"', `salario` = '"+ salarioIn->text() +"', `nombre` = '"+ nombreEmpIn->text() +"' WHERE (`idempleado` = '"+ idAEditar +"')");
        if(!editQry.exec())
            QMessageBox::critical(this, "ERROR ", editQry.lastError().text());
    }


    on_hidePopUp_clicked();
    model->select();
}

void MainWindow::on_citasPorFecha_clicked()
{
    QString fechaCitas = ui->fechaTabla->date().toString("yyyy-MM-dd");
    QSqlQueryModel *citasModel = new QSqlQueryModel;
    QSqlQuery qry;
    qry.prepare("SELECT c.idcita, c.fecha, c.hora, e.nombre AS `Empleado`, cl.nombre AS `Cliente`, COUNT(spc.idcita) AS `# serv agendados` FROM cita AS c INNER JOIN empleado AS e ON c.idempleado = e.idempleado INNER JOIN cliente AS cl ON c.idcliente = cl.idcliente LEFT JOIN servicioporcita AS spc ON c.idcita = spc.idcita WHERE c.fecha = '"+ fechaCitas +"' GROUP BY c.idcita ORDER BY c.hora");
    qry.exec();
    citasModel->setQuery(qry);
    ui->citasTable->setModel(citasModel);
}



void MainWindow::on_agendarBtn_clicked()
{
    QString fecha = ui->fechaCitaIn->date().toString("yyyy-MM-dd");
    QString horario = ui->horarioCitaIn->time().toString();
    QString empleado = ui->empleadoCitaIn->currentText();
    QString cliente = ui->clienteCitaIn->currentText();

    QSqlQuery agendarQry;
    agendarQry.prepare("INSERT INTO cita (fecha, hora, idempleado, idcliente) VALUES ('"+ fecha +"', '"+ horario +"', (SELECT idempleado FROM empleado WHERE nombre = '"+ empleado +"'), (SELECT idcliente FROM cliente WHERE nombre = '"+ cliente +"'))");
    if(!agendarQry.exec())
        QMessageBox::critical(this, "ERROR ", agendarQry.lastError().text());
    else{
       QMessageBox::information(this, "","Cita agendada correctamente");

       QItemSelectionModel *selections = ui->serviciosTableIn->selectionModel();
       QModelIndexList selected = selections->selectedRows();

       for( int i = 0 ; i < selected.size(); i++ ) {

           QString idServ = selected[i].data().toString();
           QSqlQuery servicioQry;

           servicioQry.prepare("INSERT INTO servicioporcita (idcita, idservicio) VALUES ((SELECT MAX(idcita) FROM cita), '"+ idServ +"')");

           if(!servicioQry.exec())
               QMessageBox::critical(this, "ERROR", servicioQry.lastError().text());

           ui->fechaTabla->setDate(ui->fechaCitaIn->date());
           on_citasPorFecha_clicked();

       }
    }
}

void MainWindow::on_citasTable_clicked(const QModelIndex &index)
{
    QItemSelectionModel *slct = ui->citasTable->selectionModel();

    if(slct->hasSelection()){
        ui->eliminarCita->setVisible(true);
    }else{
        ui->eliminarCita->setVisible(false);
    }
}

void MainWindow::on_eliminarCita_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "¿Está seguro de que desea desagendar?", "Se eliminará esta cita.",
                                    QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {

        QItemSelectionModel *selections = ui->citasTable->selectionModel();
        QModelIndexList selected = selections->selectedRows();

        for( int i = 0 ; i < selected.size(); i++ ) {

            QString idToDelete = selected[i].data().toString();
            QSqlQuery deletionQry;

            deletionQry.prepare("DELETE FROM cita WHERE idcita = " + idToDelete);

            if(!deletionQry.exec())
                QMessageBox::critical(this, "ERROR", deletionQry.lastError().text());
        }
    }

    on_citasPorFecha_clicked();
}

void MainWindow::on_tableCitasCobro_clicked(const QModelIndex &index)
{

    QItemSelectionModel *select = ui->tableCitasCobro->selectionModel();
    QString idCita = select->selectedRows(0).at(0).data().toString();

    QSqlQueryModel *resumenModel = new QSqlQueryModel;
    QSqlQuery serviciosQry;
    serviciosQry.prepare("SELECT s.nombre, s.precio FROM cita AS c INNER JOIN servicioporcita AS spc ON c.idcita = spc.idcita INNER JOIN servicio AS s ON s.idservicio = spc.idservicio WHERE c.idcita = '"+ idCita +"'");

    if(!serviciosQry.exec())
        QMessageBox::critical(this, "ERROR", serviciosQry.lastError().text());

    resumenModel->setQuery(serviciosQry);
    ui->tableResumen->setModel(resumenModel);

    QString total;
     QSqlQuery query("SELECT SUM(s.precio) FROM cita AS c INNER JOIN servicioporcita AS spc ON c.idcita = spc.idcita INNER JOIN servicio AS s ON s.idservicio = spc.idservicio WHERE c.idcita = '"+ idCita +"'");
     while (query.next()) {
         total.clear();
         total.append("$" + query.value(0).toString());
     }
     ui->totalLbl->setText(total);




}
