#pragma once

#include <QDialog>
#include <QKeySequence>
#include <QTableWidget>

class ControlsSettingDialog : public QDialog {
public:
    ControlsSettingDialog(QWidget* parent = nullptr);
    ~ControlsSettingDialog();

private:
    void apply();
    void add_shortcut_items(QTableWidget* table, int vk_array[], int count);
};