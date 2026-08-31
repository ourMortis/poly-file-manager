#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include "appconfig.hpp"

#include <QDialog>
#include <QString>
#include <QStringList>
#include <vector>

class QTableWidget;

/// A single setting row: a config key, a display name and the preset choices.
struct SettingDef
{
    QString key;
    QString label;
    QStringList options;   // empty -> free-text input (QLineEdit)
    QString defaultValue;  // prefill for free-text settings when config is empty
    bool numeric = false;  // the value must be a positive integer on save
};

/**
 * @brief Generic settings editor.
 *
 * Shows one row per setting (name | value) where the value is chosen from a
 * preset list via a combo box. "保存" writes the values to the JSON config and
 * emits settingsSaved(); "关闭" discards changes.
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

  public:
    SettingsDialog(AppConfig &config, const std::vector<SettingDef> &defs, QWidget *parent = nullptr);

  signals:
    void settingsSaved();

  private slots:
    void saveAndClose();

  private:
    AppConfig &config_;
    const std::vector<SettingDef> defs_;
    QTableWidget *table_ = nullptr;
};

#endif // SETTINGSDIALOG_HPP
