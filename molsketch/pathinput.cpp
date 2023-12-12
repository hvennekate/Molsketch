/***************************************************************************
 *   Copyright (C) 2021 by Hendrik Vennekate, Hendrik.Vennekate@posteo.de  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#include "pathinput.h"
#include "ui_pathinput.h"

#include <QFileDialog>

PathInput::PathInput(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::PathInput)
{
  ui->setupUi(this);
  connect(ui->path, &QLineEdit::textChanged, this, &PathInput::pathStringChanged);
}

PathInput::~PathInput() {
  delete ui;
}

QString PathInput::value() const {
  return ui->path->text();
}

QString PathInput::label() const {
  return ui->path->placeholderText();
}

PathInput *PathInput::fileInput(const QString &extension, QWidget *parent) {
  return new FilePathInput(extension, parent);
}

PathInput *PathInput::folderInput(QWidget *parent) {
  return new FolderPathInput(parent);
}

void PathInput::setValue(const QString &value) {
  ui->path->setText(value);
}

void PathInput::setLabel(const QString &value) {
  ui->path->setPlaceholderText(value);
}

void PathInput::on_dialogButton_clicked() {
  auto newValue = getValueFromDialog();
  if (!newValue.isEmpty()) setValue(newValue);
}

QString FilePathInput::getValueFromDialog() const {
  return QFileDialog::getOpenFileName(nullptr, label(), value(), extension);
}

FilePathInput::FilePathInput(const QString &extension, QWidget *parent)
  : PathInput(parent),
    extension(extension) {}

void FilePathInput::setExtensionFilter(const QString &extension) {
  this->extension = extension;
}

QString FolderPathInput::getValueFromDialog() const {
  return QFileDialog::getExistingDirectory(nullptr, label(), value());
}

FolderPathInput::FolderPathInput(QWidget *parent)
  : PathInput(parent) {}
