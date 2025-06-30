#include "ControlsSettingDialog.h"
#include "VirtualKey.h"
#include <QDialogButtonBox>
#include <QFrame>
#include <QHeaderView>
#include <QKeyEvent>
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

    nes_table_ = new QTableWidget(this);
    tabs->addTab(nes_table_, "Nes");

    nes_table_->setColumnCount(3);
    nes_table_->setHorizontalHeaderLabels(
        { tr("Action"), tr("Current Shortcut"), tr("Default Shortcut") });

    nes_table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    nes_table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    nes_table_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    nes_table_->verticalHeader()->setVisible(false);
    nes_table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    nes_table_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    nes_table_->setSelectionMode(QAbstractItemView::SingleSelection);

    nes_table_->installEventFilter(this);

    add_shortcut_items(nes_table_, nes_key_group.keys.data(), nes_key_group.keys.size());

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

        auto vk_name_item = new QTableWidgetItem(vk_names[vk]);
        vk_name_item->setData(Qt::UserRole, vk);

        table->setItem(i, 0, vk_name_item);
        if (settings.contains(vk_names[vk])) {
            table->setItem(i, 1, new QTableWidgetItem(settings.value(vk_names[vk]).toString()));
        }
        table->setItem(i, 2, new QTableWidgetItem(default_keys[vk].toString()));
    }

    settings.endGroup();
}

void ControlsSettingDialog::apply() {
    save_settings(nes_table_);
    accept();
}

void ControlsSettingDialog::save_settings(QTableWidget* table) {
    QSettings settings;
    settings.beginGroup("Key Bindings");

    auto count = table->rowCount();
    for (int i = 0; i < count; i++) {
        int vk = nes_table_->item(i, 0)->data(Qt::UserRole).toInt();
        auto key_item = nes_table_->item(i, 1);
        if (key_item) {
            auto key_text = key_item->text();

            if (key_text != "") {
                settings.setValue(vk_names[vk], QKeySequence(key_text).toString());

            } else {
                settings.remove(vk_names[vk]);
            }
        }
    }
    settings.endGroup();
}

bool ControlsSettingDialog::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* key_event = static_cast<QKeyEvent*>(event);
        if (!key_event->isAutoRepeat()) {

            if (obj == nes_table_) {
                int current_row = nes_table_->currentRow();
                int vk = nes_table_->item(current_row, 0)->data(Qt::UserRole).toInt();

                QKeySequence key_seq(key_event->modifiers() | key_event->key());
                nes_table_->setItem(current_row, 1, new QTableWidgetItem(key_seq.toString()));

                return true;
            }
        }
    }
    return QDialog::eventFilter(obj, event);
}
