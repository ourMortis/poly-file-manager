#ifndef IMPORTDIALOG_HPP
#define IMPORTDIALOG_HPP

#include <QDialog>
#include <QString>

class QCheckBox;
class QLineEdit;

/**
 * @brief Options shown before importing paths from a folder.
 *
 * Lets the user decide whether the selected folder itself is imported, whether
 * sub-folders are searched recursively, and whether a tag should be applied to
 * every imported path (leave it empty to import without tagging).
 */
class ImportDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit ImportDialog(QWidget *parent = nullptr);

    bool includeFolder() const;
    bool recursive() const;
    QString tagName() const;

  private:
    QCheckBox *include_folder_ = nullptr;
    QCheckBox *recursive_ = nullptr;
    QLineEdit *tag_edit_ = nullptr;
};

#endif // IMPORTDIALOG_HPP
