#ifndef JSHIGHLIGHTER_H
#define JSHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextFormat>
#include <QStringList>
#include <QRegExp>
#include <QVector>
#include <QTextCharFormat>

// incomplete example from
// http://www.qtcentre.org/threads/25501-QtScript-script-text-editor-for-Qt-4-4-3-with-Syntax-Highlighter

class JSHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    void SetRule(QString kind, QString pattern, QTextCharFormat format)
    {
        HighlightingRule rule;
        rule.pattern = QRegExp(pattern);
        rule.format = format;
        highlightingRules.append(rule);
    }

    JSHighlighter(QTextDocument *parent = 0): QSyntaxHighlighter(parent)
    {
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(Qt::black);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns;
        keywordPatterns << "\\bvar\\b" << "\\bArray\\b" << "\\bfunction\\b"
                        << "\\breturn\\b" << "\\barguments\\b" << "\\bif\\b"
                        << "\\belse\\b" << "\\bfor\\b" << "\\bswitch\\b"
                        << "\\bcase\\b" << "\\bbreak\\b" << "\\bwhile\\b";
        int i = 0;
        foreach (const QString &pattern, keywordPatterns) {
            SetRule(QString("00_KeyWord_%1").arg(i),pattern,keywordFormat);
            ++i;
        }
        // Values
        QTextCharFormat valueFormat, classFormat;
        valueFormat.setForeground(Qt::blue);
        SetRule("03_Values","\\btrue\\b|\\bfalse\\b|\\b[0-9]+\\b",valueFormat);
        QTextCharFormat functionFormat;
        //functionFormat.setFontItalic(false);
        functionFormat.setForeground(Qt::darkBlue);
        SetRule("04_Functions","\\b[A-Za-z0-9_]+(?=\\()",functionFormat);
        // Qt Classes
        classFormat.setFontWeight(QFont::Bold);
        classFormat.setForeground(Qt::darkMagenta);
        SetRule("06_QtClasses","\\bQ[A-Z]+[A-Za-z]+\\b",classFormat);
        // Quotation
        QTextCharFormat quotationFormat;
        quotationFormat.setForeground(Qt::blue);
        SetRule("z1_Quotations","\"[^\"]*\"",quotationFormat);
        // Single Line Comments
        QTextCharFormat singleLineCommentFormat;
        singleLineCommentFormat.setForeground(Qt::darkGreen);
        SetRule("z2_SingleLineComments","//[^\n]*",singleLineCommentFormat);
    }
};

#endif // JSHIGHLIGHTER_H
