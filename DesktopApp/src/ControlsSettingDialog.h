#pragma once

#include "VirtualKey.h"
#include <QDialog>
#include <QKeySequence>
#include <QTabWidget>
#include <QTableWidget>

class ControlsSettingDialog : public QDialog {
public:
    ControlsSettingDialog(QWidget* parent = nullptr);
    ~ControlsSettingDialog();

private:
    void apply();
    void add_shortcut_items(QTableWidget* table, const int vk_array[], int count);

    QTableWidget* tables_[2];

    QTableWidget* create_table(const VirtualKeyGroup& vk_group, QTabWidget*);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void save_settings(QTableWidget* table);
};