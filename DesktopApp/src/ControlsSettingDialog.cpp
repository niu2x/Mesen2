#include "ControlsSettingDialog.h"
#include "VirtualKey.h"
#include <QDialogButtonBox>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

ControlsSettingDialog::ControlsSettingDialog(QWidget* parent)
    : QDialog(parent) {
    auto tabs = new QTabWidget(this);
    auto vbox = new QVBoxLayout(this);
    setLayout(vbox);
    vbox->addWidget(tabs);

    QDialogButtonBox* button_box
        = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel);

    vbox->addWidget(button_box);
    QObject::connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(button_box->button(QDialogButtonBox::Apply),
                     &QPushButton::clicked,
                     this,
                     &ControlsSettingDialog::apply);

    auto table = new QTableWidget(this);
    tabs->addTab(table, "Nes");

    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(
        { tr("Action"), tr("Current Shortcut"), tr("Default Shortcut") });

    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    add_shortcut_items(table, nes_key_group.keys.data(), nes_key_group.keys.size());

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    adjustSize();
}

ControlsSettingDialog::~ControlsSettingDialog() { }

void ControlsSettingDialog::add_shortcut_items(QTableWidget* table, int vk_array[], int count) {
    table->setRowCount(count);

    QSettings settings;
    settings.beginGroup("Key Bindings");

    for (int i = 0; i < count; i++) {
        int vk = vk_array[i];
        table->setItem(i, 0, new QTableWidgetItem(vk_names[vk]));
        if (settings.contains(vk_names[vk])) {
            table->setItem(i, 1, new QTableWidgetItem(settings.value(vk_names[vk]).toString()));
        }
        table->setItem(i, 2, new QTableWidgetItem(default_keys[vk].toString()));
    }

    settings.endGroup();
}

void ControlsSettingDialog::apply() { accept(); }
