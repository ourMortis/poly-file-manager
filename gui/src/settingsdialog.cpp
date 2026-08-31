#include "settingsdialog.hpp"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIntValidator>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(AppConfig &config, const std::vector<SettingDef> &defs, QWidget *parent)
    : QDialog(parent), config_(config), defs_(defs)
{
    setWindowTitle("设置");
    resize(440, 220);

    auto *layout = new QVBoxLayout(this);

    table_ = new QTableWidget(static_cast<int>(defs_.size()), 2, this);
    table_->setHorizontalHeaderLabels({QStringLiteral("设置项"), QStringLiteral("值")});
    table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table_->verticalHeader()->setVisible(false);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);

    for (int row = 0; row < static_cast<int>(defs_.size()); ++row)
    {
        const auto &def = defs_[row];
        auto *name = new QTableWidgetItem(def.label);
        name->setFlags(name->flags() & ~Qt::ItemIsEditable);
        table_->setItem(row, 0, name);

        const QString current = QString::fromStdString(config_.setting(def.key.toStdString()));
        if (def.options.isEmpty())
        {
            auto *edit = new QLineEdit(table_);
            edit->setText(current.isEmpty() ? def.defaultValue : current);
            if (def.numeric)
            {
                edit->setValidator(new QIntValidator(1, 100000, edit));
            }
            table_->setCellWidget(row, 1, edit);
        }
        else
        {
            auto *combo = new QComboBox(table_);
            combo->addItems(def.options);
            const int index = current.isEmpty() ? 0 : def.options.indexOf(current);
            combo->setCurrentIndex(index >= 0 ? index : 0);
            table_->setCellWidget(row, 1, combo);
        }
    }
    layout->addWidget(table_);

    auto *buttons = new QHBoxLayout;
    auto *save_btn = new QPushButton("保存", this);
    auto *close_btn = new QPushButton("关闭", this);
    buttons->addStretch();
    buttons->addWidget(save_btn);
    buttons->addWidget(close_btn);
    layout->addLayout(buttons);

    connect(save_btn, &QPushButton::clicked, this, &SettingsDialog::saveAndClose);
    connect(close_btn, &QPushButton::clicked, this, &QDialog::reject);
}

void SettingsDialog::saveAndClose()
{
    bool invalid = false;
    for (int row = 0; row < static_cast<int>(defs_.size()); ++row)
    {
        if (auto *combo = qobject_cast<QComboBox *>(table_->cellWidget(row, 1)))
        {
            config_.setSetting(defs_[row].key.toStdString(), combo->currentText().toStdString());
        }
        else if (auto *edit = qobject_cast<QLineEdit *>(table_->cellWidget(row, 1)))
        {
            const QString text = edit->text().trimmed();
            bool ok = false;
            const int value = text.toInt(&ok);
            if (defs_[row].numeric && (!ok || value <= 0))
            {
                invalid = true;
                continue; // keep the previous value for this setting
            }
            config_.setSetting(defs_[row].key.toStdString(), text.toStdString());
        }
    }
    if (invalid)
    {
        QMessageBox::warning(this, "设置", "存在非法输入，相应设置项未生效（已保留原值）。");
    }
    emit settingsSaved();
    accept();
}
