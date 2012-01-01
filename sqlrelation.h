/**************************************
* header SqlRelation Class for Qt     *
* Allow to make LEFT/OUTER/RIGHT join *
* Licence: Creatives Commons BY-NC-SA *
*    @author: Pierre-Etienne Crépy    *
*    @mail: pierre-etienne@crepy.eu   *
***************************************/

#ifndef SQLRELATION_H
#define SQLRELATION_H

#include <QSqlRelation>

class SqlRelation : public QSqlRelationalTableModel, public QSqlRelation
{
    Q_OBJECT

     Q_ENUMS( joinKind )
     Q_PROPERTY( joinKind joinKind READ getJoinKind WRITE setJoinKind )

public:

    enum joinKind { LEFT, RIGHT, INNER, OUTER };

    SqlRelation(const QString& , const QString& , const QString& , const joinKind = SqlRelation::RIGHT);
    SqlRelation(const QSqlRelation&);
    SqlRelation(const SqlRelation&);
    SqlRelation();
    SqlRelation operator=(const SqlRelation&);

    joinKind getJoinKind() const;
    QString tableName() const;
    QString indexColumn() const;
    QString displayColumn() const;

    void setJoinKind(joinKind );
    void setTableName(QString );
    void setiColumn(QString );
    void setdColumn(QString );

    bool isValid() const;

private:
    QString tName;
    QString iColumn;
    QString dColumn;
    joinKind jKind;

};

#endif // SQLRELATION_H
