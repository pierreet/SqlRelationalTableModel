#ifndef SQLRELATIONALTABLEMODEL_H
#define SQLRELATIONALTABLEMODEL_H

#include <QSqlRelationalTableModel>
#include <SqlRelation.h>

class SqlRelationalTableModel : public QSqlRelationalTableModel {
public:

    struct SRelation
    {
        public:

            SRelation(): model(0),m_parent(0),m_dictInitialized(false){}

            void init(SqlRelationalTableModel *parent, const SqlRelation &relation){
                m_parent = parent;
                rel = relation;
                }

            void populateModel(){
                if (!isValid())
                    return;

                if (!model) {
                    model = new SqlRelationalTableModel(m_parent, m_parent->database());
                    model->setTable(rel.tableName());
                    model->select();
                }
            }

            bool isDictionaryInitialized(){return m_dictInitialized;}

            void populateDictionary(){
                if (!isValid())
                    return;

                if (model ==  NULL)
                    populateModel();

                QSqlRecord record;
                QString indexColumn;
                QString displayColumn;
                for (int i=0; i < model->rowCount(); ++i) {
                    record = model->record(i);

                    indexColumn = rel.indexColumn();
                    if (m_parent->database().driver()->isIdentifierEscaped(indexColumn, QSqlDriver::FieldName))
                        indexColumn = m_parent->database().driver()->stripDelimiters(indexColumn, QSqlDriver::FieldName);

                    displayColumn = rel.displayColumn();
                    if (m_parent->database().driver()->isIdentifierEscaped(displayColumn, QSqlDriver::FieldName))
                        displayColumn = m_parent->database().driver()->stripDelimiters(displayColumn, QSqlDriver::FieldName);

                    dictionary[record.field(indexColumn).value().toString()] =
                        record.field(displayColumn).value();
                }
                m_dictInitialized = true;
            }

            void clearDictionary(){
                dictionary.clear();
                m_dictInitialized = false;
            }

            void clear(){
                delete model;
                model = 0;
                clearDictionary();
            }

            bool isValid(){return (rel.isValid() && m_parent != NULL);}

            SqlRelation rel;
            SqlRelationalTableModel *model;
            QHash<QString, QVariant> dictionary;

        private:
            SqlRelationalTableModel *m_parent;
            bool m_dictInitialized;
    };


  SqlRelationalTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

  void setEditableFlagForColumn(int column, bool editable);
  Qt::ItemFlags flags ( const QModelIndex & index ) const;
  void clear();
  void setGroupBy(QString grpby);
  void setSort(int column, Qt::SortOrder order);
  void clearAdditionalField();
  void addAdditionalField(QString s);
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  void clearChanges();
  void setRelation(int column, const SqlRelation &relation);
  SqlRelation relation(int column) const;

protected:

  virtual QString selectStatement() const;
  QSqlTableModel * relationModel(int column) const;
  QString orderByClause() const;

private:

  QSet<int> readOnlyColumns;
  mutable QVector<SRelation> relations;
  QString groupByClause;
  QString additionalField;
  int sortColumn;
  Qt::SortOrder sortOrder;

};

#endif // SQLRELATIONALTABLEMODEL_H
