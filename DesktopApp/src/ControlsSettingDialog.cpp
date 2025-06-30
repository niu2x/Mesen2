#include "ControlsSettingDialog.h"
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

    tables_[0] = create_table(nes_key_group, tabs);
    tables_[1] = create_table(misc_key_group, tabs);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    adjustSize();
}

QTableWidget* ControlsSettingDialog::create_table(const VirtualKeyGroup& vk_group,
                                                  QTabWidget* tabs) {
    auto table = new QTableWidget(this);
    tabs->addTab(table, vk_group.group_name.c_str());

    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(
        { tr("Action"), tr("Current Shortcut"), tr("Default Shortcut") });

    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    table->setSelectionMode(QAbstractItemView::SingleSelection);

    table->installEventFilter(this);

    add_shortcut_items(table, vk_group.keys.data(), vk_group.keys.size());
    return table;
}

ControlsSettingDialog::~ControlsSettingDialog() { }

void ControlsSettingDialog::add_shortcut_items(QTableWidget* table,
                                               const int vk_array[],
                                               int count) {
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
    save_settings(tables_[0]);
    save_settings(tables_[1]);
    accept();
}

void ControlsSettingDialog::save_settings(QTableWidget* table) {
    QSettings settings;
    settings.beginGroup("Key Bindings");

    auto count = table->rowCount();
    for (int i = 0; i < count; i++) {
        int vk = table->item(i, 0)->data(Qt::UserRole).toInt();
        auto key_item = table->item(i, 1);
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

            for (int i = 0; i < 2; i++) {
                auto table = tables_[i];

                if (obj == table) {
                    int current_row = table->currentRow();
                    int vk = table->item(current_row, 0)->data(Qt::UserRole).toInt();
                    QKeySequence key_seq(key_event->modifiers() | key_event->key());
                    table->setItem(current_row, 1, new QTableWidgetItem(key_seq.toString()));
                    return true;
                }
            }
        }
    }
    return QDialog::eventFilter(obj, event);
}
