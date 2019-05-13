#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QtCore>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QItemSelectionModel>

void MainWindow::on_btnCrearMiembro_clicked()
{
    if(currentTable == "clientes_miembros")
        ui->popupTitle->setText("Crear nuevo cliente");
    else
        ui->popupTitle->setText("Crear nuevo " + currentTable);
    ui->popup->setVisible(true);
    ui->addButton->setVisible(true);
    ui->editButton->setVisible(false);
    ui->cancelButton->setVisible(true);

    // CREATE ITEM



}

void MainWindow::on_btnEditarMiembro_clicked()
{
    if(currentTable == "clientes_miembros")
        ui->popupTitle->setText("Ver / Editar cliente");
    else
        ui->popupTitle->setText("Ver / Editar " + currentTable);
    ui->popup->setVisible(true);
    ui->addButton->setVisible(false);
    ui->editButton->setVisible(true);
    ui->cancelButton->setVisible(true);

    int selectedRow = ui->tableMiembro->selectionModel()->selectedRows()[0].row();

    if(currentTable == "servicio"){
        precioIn->setText(ui->tableMiembro->model()->index(selectedRow, 1).data().toString());
        descripcionIn->setText(ui->tableMiembro->model()->index(selectedRow, 2).data().toString());
        nombreServIn->setText(ui->tableMiembro->model()->index(selectedRow, 3).data().toString());
    } else if(currentTable == "empleado"){
        rolIn->setText(ui->tableMiembro->model()->index(selectedRow, 1).data().toString());
        entradaIn->setTime(QTime().fromString(ui->tableMiembro->model()->index(selectedRow, 2).data().toString()));
        salidaIn->setTime(QTime().fromString(ui->tableMiembro->model()->index(selectedRow, 3).data().toString()));
        salarioIn->setText(ui->tableMiembro->model()->index(selectedRow, 4).data().toString());
        nombreEmpIn->setText(ui->tableMiembro->model()->index(selectedRow, 5).data().toString());
    } else if(currentTable == "clientes_miembros"){
        if(ui->tableMiembro->model()->index(selectedRow, 3).data().toString() != "0"){
            esMiembro->setChecked(true);
            fechaIn->setDate(QDate().fromString(ui->tableMiembro->model()->index(selectedRow, 4).data().toString(), "yyyy-MM-dd"));
            puntosIn->setText(ui->tableMiembro->model()->index(selectedRow, 5).data().toString());
        } else{
            esMiembro->setChecked(false);
        }

        nombreIn->setText(ui->tableMiembro->model()->index(selectedRow, 1).data().toString());
        telefonoIn->setText(ui->tableMiembro->model()->index(selectedRow, 2).data().toString());

    }

}

void MainWindow::on_btnEliminarMiembro_clicked()
{

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "¿Está seguro de que desea eliminar este item?", "Se eliminará este y todos los relacionados a este en otras tablas.",
                                    QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {

        QString id = model->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString();

        QItemSelectionModel *selections = ui->tableMiembro->selectionModel();
        QModelIndexList selected = selections->selectedRows();

        for( int i = 0 ; i < selected.size(); i++ ) {

            QString idToDelete = selected[i].data().toString();
            QSqlQuery deletionQry;

            if(currentTable == "clientes_miembros"){
                deletionQry.prepare("DELETE FROM cliente WHERE " + id + " = " + idToDelete);
            }else{
                deletionQry.prepare("DELETE FROM " + currentTable + " WHERE " + id + " = " + idToDelete);
            }

            if(!deletionQry.exec())
                QMessageBox::critical(this, "ERROR", deletionQry.lastError().text());
        }
    }

    model->select();
    on_btnDeseleccionar_clicked();
}

void MainWindow::on_btnDeseleccionar_clicked()
{
    ui->tableMiembro->clearSelection();
    QModelIndex nothing;
    on_tableMiembro_clicked(nothing);
}

void MainWindow::on_tableMiembro_clicked(const QModelIndex &index)
{
    QItemSelectionModel *select = ui->tableMiembro->selectionModel();

    if(select->hasSelection()){
        ui->btnEliminarMiembro->setVisible(true);
        ui->btnCrearMiembro->setVisible(false);

        if(select->selectedRows().count() == 1){
            ui->btnEditarMiembro->setVisible(true);
            ui->btnDeseleccionar->setVisible(false);
        } else{
            ui->btnEditarMiembro->setVisible(false);
            ui->btnDeseleccionar->setVisible(true);
        }

    }else{
        ui->btnDeseleccionar->setVisible(false);
        ui->btnEliminarMiembro->setVisible(false);
        ui->btnEditarMiembro->setVisible(false);
        ui->btnCrearMiembro->setVisible(true);
    }

}
