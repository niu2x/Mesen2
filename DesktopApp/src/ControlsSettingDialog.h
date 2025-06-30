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

    QTableWidget* nes_table_;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void save_settings(QTableWidget* table);
};