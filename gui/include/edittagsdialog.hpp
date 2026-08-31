#ifndef EDITTAGSDIALOG_HPP
#define EDITTAGSDIALOG_HPP

#include <QDialog>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QVector>
#include <functional>

class QLineEdit;
class QTreeWidget;
class QTreeWidgetItem;

/**
 * @brief Tree editor for the tag assignment of the selected files.
 *
 * Each tag is a top-level row with a checkbox and an expandable branch. Expanding
 * a tag reveals the selected files, each with its own checkbox. Checking the tag
 * row toggles every file for that tag at once; toggling an individual file row
 * only changes that file. A text input can add a brand-new tag to all files.
 */
class EditTagsDialog : public QDialog
{
    Q_OBJECT

  public:
    EditTagsDialog(const QStringList &tags, const QStringList &files,
                   const std::function<bool(const QString &, const QString &)> &isAssigned,
                   QWidget *parent = nullptr);

    /// @return (tag, file full path) pairs that should end up assigned.
    [[nodiscard]] QVector<QPair<QString, QString>> checkedPairs() const;
    /// @return A brand-new tag to assign to every file (empty if none).
    [[nodiscard]] QString newTag() const;

  private slots:
    void onItemChanged(QTreeWidgetItem *item, int column);

  private:
    QTreeWidget *list_ = nullptr;
    QLineEdit *new_tag_ = nullptr;
    QStringList files_;
};

#endif // EDITTAGSDIALOG_HPP
