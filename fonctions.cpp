#include "fonctions.h"

QString relationField(QString tableName, QString fieldName)
{
    QString ret;
    ret.reserve(tableName.size() + fieldName.size() + 1);
    ret.append(tableName).append(QLatin1Char('.')).append(fieldName);

    return ret;
}

QString qAppendWhereClause(QString query, QString clause1, QString clause2)
{
    if (clause1.isEmpty() && clause2.isEmpty())
        return query;
    if (clause1.isEmpty() || clause2.isEmpty())
        query.append(QLatin1String(" WHERE (")).append(clause1).append(clause2);
    else
        query.append(QLatin1String(" WHERE (")).append(clause1).append(QLatin1String(") AND (")).append(clause2);

    query.append(QLatin1String(") "));

    return query;
}

