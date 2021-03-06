/* This file is part of the KDE project
 * Copyright (C) 2005, 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2012 Shreya Pandit <shreya@shreyapandit.com>
 * Copyright (C) 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWStatisticsWidget.h"

#include "Words.h"
#include "KWDocument.h"
#include "frames/KWFrame.h"
#include "frames/KWFrameSet.h"
#include "frames/KWTextFrameSet.h"
#include "dockers/StatisticsPreferencesPopup.h"
#include <KoCanvasResourceManager.h>
#include <KoSelection.h>
#include <KoShape.h>
#include <KoIcon.h>

#include <QTextLayout>
#include <QTextDocument>
#include <QTextBlock>
#include <QTimer>

KWStatisticsWidget::KWStatisticsWidget(QWidget *parent)
        : QWidget(parent),
          m_resourceManager(0),
          m_selection(0),
          m_document(0),
          m_textDocument(0),
          m_timer(0),
          m_words(0),
          m_sentences(0),
          m_syllables(0),
          m_cjkChars(0),
          m_charsWithSpace(0),
          m_charsWithoutSpace(0),
          m_lines(0),
          m_paragraphs(0)
{
    m_timer = new QTimer(this);
    initUi();
    initLayout();
    m_menu = new StatisticsPreferencesPopup(m_preferencesButton);
    m_preferencesButton->setMenu(m_menu);
    m_preferencesButton->setPopupMode(QToolButton::InstantPopup);
    m_preferencesButton->setIcon(koIcon("configure"));

    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateData()));

    connect(m_preferencesButton, SIGNAL(clicked()), m_preferencesButton, SLOT(showMenu()));
    connect(m_menu, SIGNAL(wordsDisplayChange(int)), this, SLOT(wordsDisplayChanged(int)));
    connect(m_menu, SIGNAL(sentencesDisplayChange(int)), this, SLOT(sentencesDisplayChanged(int)));
    connect(m_menu, SIGNAL(linesDisplayChange(int)), this, SLOT(linesDisplayChanged(int)));
    connect(m_menu, SIGNAL(syllablesDisplayChange(int)), this, SLOT(syllablesDisplayChanged(int)));
    connect(m_menu, SIGNAL(charspaceDisplayChange(int)), this, SLOT(charspaceDisplayChanged(int)));
    connect(m_menu, SIGNAL(charnospaceDisplayChange(int)), this, SLOT(charnospaceDisplayChanged(int)));
    connect(m_menu, SIGNAL(eastDisplayChange(int)), this, SLOT(eastDisplayChanged(int)));
    connect(m_menu, SIGNAL(fleschDisplayChange(int)), this, SLOT(fleschDisplayChanged(int)));

    KConfigGroup cfgGroup = KGlobal::config()->group("Statistics");
    bool visible = cfgGroup.readEntry("WordsVisible", true);
    m_wordsLabel->setVisible(visible);
    m_countWords->setVisible(visible);
    if (visible) {
        m_menu->w->check_words->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("SentencesVisible", true);
    m_sentencesLabel->setVisible(visible);
    m_countSentences->setVisible(visible);
    if (visible) {
        m_menu->w->check_sentences->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("SyllablesVisible", true);
    m_syllablesLabel->setVisible(visible);
    m_countSyllables->setVisible(visible);
    if (visible) {
        m_menu->w->check_syllables->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("LinesVisible", true);
    m_linesLabel->setVisible(visible);
    m_countLines->setVisible(visible);
    if (visible) {
        m_menu->w->check_lines->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("EastAsianCharactersVisible", true);
    m_cjkcharsLabel->setVisible(visible);
    m_countCjkchars->setVisible(visible);
    if (visible) {
        m_menu->w->check_east->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("FleschVisible", true);
    m_fleschLabel->setVisible(visible);
    m_countFlesch->setVisible(visible);
    if (visible) {
        m_menu->w->check_flesch->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("CharspacesVisible", true);
    m_spacesLabel->setVisible(visible);
    m_countSpaces->setVisible(visible);
    if (visible) {
        m_menu->w->check_charspace->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("CharnospacesVisible", true);
    m_nospacesLabel->setVisible(visible);
    m_countNospaces->setVisible(visible);
    if (visible) {
        m_menu->w->check_charnospace->setCheckState(Qt::Checked);
    }
}

KWStatisticsWidget::~KWStatisticsWidget()
{
    m_timer->stop();
}


void KWStatisticsWidget::initUi()
{
    m_wordsLabel = new QLabel(i18n("Words:"));
    m_countWords = new QLabel;

    m_sentencesLabel = new QLabel(i18n("Sentences:"));
    m_countSentences = new QLabel;

    m_syllablesLabel = new QLabel(i18n("Syllables:"));
    m_countSyllables = new QLabel;

    m_spacesLabel = new QLabel(i18n("Characters (spaces):"));
    m_countSpaces = new QLabel;

    m_nospacesLabel = new QLabel(i18n("Characters (no spaces):"));
    m_countNospaces = new QLabel;

    m_linesLabel = new QLabel(i18n("Lines:"));
    m_countLines = new QLabel;

    m_fleschLabel = new QLabel(i18n("Readability:"));
    m_countFlesch = new QLabel;
    m_fleschLabel->setToolTip(i18n("Flesch reading ease"));

    m_cjkcharsLabel = new QLabel(i18n("East asian characters:"));
    m_countCjkchars = new QLabel;

    m_preferencesButton = new QToolButton;
}

void KWStatisticsWidget::initLayout()
{
    m_mainBox = new QBoxLayout(QBoxLayout::LeftToRight, this);

    m_wordsLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_wordsLayout);
    m_wordsLayout->addWidget(m_wordsLabel);
    m_wordsLayout->addWidget(m_countWords);

    m_sentencesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_sentencesLayout);
    m_sentencesLayout->addWidget(m_sentencesLabel);
    m_sentencesLayout->addWidget(m_countSentences);

    m_syllablesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_syllablesLayout);
    m_syllablesLayout->addWidget(m_syllablesLabel);
    m_syllablesLayout->addWidget(m_countSyllables);

    m_cjkcharsLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_cjkcharsLayout);
    m_cjkcharsLayout->addWidget(m_cjkcharsLabel);
    m_cjkcharsLayout->addWidget(m_countCjkchars);

    m_spacesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_spacesLayout);
    m_spacesLayout->addWidget(m_spacesLabel);
    m_spacesLayout->addWidget(m_countSpaces);

    m_nospacesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_nospacesLayout);
    m_nospacesLayout->addWidget(m_nospacesLabel);
    m_nospacesLayout->addWidget(m_countNospaces);

    m_fleschLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_fleschLayout);
    m_fleschLayout->addWidget(m_fleschLabel);
    m_fleschLayout->addWidget(m_countFlesch);

    m_linesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_linesLayout);
    m_linesLayout->addWidget(m_linesLabel);
    m_linesLayout->addWidget(m_countLines);

    // The button that opens the preferences dialog.
    m_mainBox->addWidget(m_preferencesButton);

    setLayout(m_mainBox); // FIXME: Is this necessary?
}

void KWStatisticsWidget::updateData()
{
    if (!isVisible()) {
        return;
    }
    m_charsWithSpace = 0;
    m_charsWithoutSpace = 0;
    m_words = 0;
    m_sentences = 0;
    m_lines = 0;
    m_syllables = 0;
    m_paragraphs = 0;
    m_cjkChars = 0;

    // parts of words for better counting of syllables:
    // (only use reg exp if necessary -> speed up)

    QStringList subs_syl;
    subs_syl << "cial" << "tia" << "cius" << "cious" << "giu" << "ion" << "iou";
    QStringList subs_syl_regexp;
    subs_syl_regexp << "sia$" << "ely$";

    QStringList add_syl;
    add_syl << "ia" << "riet" << "dien" << "iu" << "io" << "ii";
    QStringList add_syl_regexp;
    add_syl_regexp << "[aeiouym]bl$" << "[aeiou]{3}" << "^mc" << "ism$"
    << "[^l]lien" << "^coa[dglx]." << "[^gq]ua[^auieo]" << "dnt$";

    foreach (KWFrameSet *fs, m_document->frameSets()) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs == 0) continue;

        QTextDocument *doc = tfs->document();
        QTextBlock block = doc->begin();
        while (block.isValid()) {
            m_paragraphs += 1;
            m_charsWithSpace += block.text().length();
            m_charsWithoutSpace += block.text().length() - block.text().count(QRegExp("\\s"));
            if (block.layout())
                m_lines += block.layout()->lineCount();
            m_cjkChars += countCJKChars(block.text());

            QString s = block.text();

            // Syllable and Word count
            // Algorithm mostly taken from Greg Fast's Lingua::EN::Syllable module for Perl.
            // This guesses correct for 70-90% of English words, but the overall value
            // is quite good, as some words get a number that's too high and others get
            // one that's too low.
            // IMPORTANT: please test any changes against demos/statistics.kwd
            QRegExp re("\\s+");
            const QStringList wordlist = s.split(re, QString::SkipEmptyParts);
            m_words += wordlist.count();
            re.setCaseSensitivity(Qt::CaseInsensitive);
            for (QStringList::ConstIterator it1 = wordlist.begin(); it1 != wordlist.end(); ++it1) {
                QString word1 = *it1;
                QString word = *it1;
                re.setPattern("[!?.,:_\"-]");    // clean word from punctuation
                word.remove(re);
                if (word.length() <= 3) {  // extension to the original algorithm
                    m_syllables++;
                    continue;
                }
                re.setPattern("e$");
                word.remove(re);
                re.setPattern("[^aeiouy]+");
                QStringList syls = word.split(re, QString::SkipEmptyParts);
                int word_syllables = 0;
                for (QStringList::Iterator it = subs_syl.begin(); it != subs_syl.end(); ++it) {
                    if (word.indexOf(*it, 0, Qt::CaseInsensitive) != -1)
                        word_syllables--;
                }
                for (QStringList::Iterator it = subs_syl_regexp.begin(); it != subs_syl_regexp.end(); ++it) {
                    re.setPattern(*it);
                    if (word.indexOf(re) != -1)
                        word_syllables--;
                }
                for (QStringList::Iterator it = add_syl.begin(); it != add_syl.end(); ++it) {
                    if (word.indexOf(*it, 0, Qt::CaseInsensitive) != -1)
                        word_syllables++;
                }
                for (QStringList::Iterator it = add_syl_regexp.begin(); it != add_syl_regexp.end(); ++it) {
                    re.setPattern(*it);
                    if (word.indexOf(re) != -1)
                        word_syllables++;
                }
                word_syllables += syls.count();
                if (word_syllables == 0)
                    word_syllables = 1;
                m_syllables += word_syllables;
            }
            re.setCaseSensitivity(Qt::CaseSensitive);

            block = block.next();

            // Sentence count
            // Clean up for better result, destroys the original text but we only want to count
            s = s.trimmed();
            if (s.isEmpty())
                continue;
            QChar lastchar = s.at(s.length() - 1);
            if (! s.isEmpty() && lastchar != QChar('.') && lastchar != QChar('?') && lastchar != QChar('!')) {  // e.g. for headlines
                s = s + '.';
            }
            re.setPattern("[.?!]+");         // count "..." as only one "."
            s.replace(re, ".");
            re.setPattern("\\d\\.\\d");      // don't count floating point numbers as sentences
            s.replace(re, "0,0");
            re.setPattern("[A-Z]\\.+");      // don't count "U.S.A." as three sentences
            s.replace(re, "*");
                for (int i = 0 ; i < s.length(); ++i) {
                    QChar ch = s[i];
                    if (ch == QChar('.') || ch == QChar('?') || ch == QChar('!'))
                        ++m_sentences;
            }
        }
    }
    updateDataUi();
}

void KWStatisticsWidget::setLayoutDirection(KWStatisticsWidget::LayoutDirection direction)
{
    if (direction == KWStatisticsWidget::LayoutHorizontal) {
        m_mainBox->setDirection(QBoxLayout::LeftToRight);
    } else {
        m_mainBox->setDirection(QBoxLayout::TopToBottom);
    }
}

void KWStatisticsWidget::setCanvas(KWCanvas* canvas)
{
    m_resourceManager = canvas->resourceManager();
    m_selection = canvas->shapeManager()->selection();
    m_document = canvas->document();
    m_timer->start(2500);
}

void KWStatisticsWidget::unsetCanvas()
{
    m_timer->stop();
    m_resourceManager = 0;
    m_selection = 0;
    m_document = 0;
}

void KWStatisticsWidget::updateDataUi()
{
    // calculate Flesch reading ease score:
    float flesch_score = 0;
    if (m_words > 0 && m_sentences > 0)
        flesch_score = 206.835 - (1.015 * (m_words / m_sentences)) - (84.6 * m_syllables / m_words);
    QString flesch = KGlobal::locale()->formatNumber(flesch_score);
    QString newText[8];
    newText[0] = KGlobal::locale()->formatNumber(m_words, 0);
    m_countWords->setText(newText[0]);

    newText[1] = KGlobal::locale()->formatNumber(m_sentences, 0);
    m_countSentences->setText(newText[1]);

    newText[2] = KGlobal::locale()->formatNumber(m_syllables, 0);
    m_countSyllables->setText(newText[2]);

    newText[3] = KGlobal::locale()->formatNumber(m_lines, 0);
    m_countLines->setText(newText[3]);

    newText[4] = KGlobal::locale()->formatNumber(m_charsWithSpace, 0);
    m_countSpaces->setText(newText[4]);

    newText[5] = KGlobal::locale()->formatNumber(m_charsWithoutSpace, 0);
    m_countNospaces->setText(newText[5]);

    newText[6] = KGlobal::locale()->formatNumber(m_cjkChars, 0);
    m_countCjkchars->setText(newText[6]);

    newText[7] = flesch;
    m_countFlesch->setText(newText[7]);
}


void KWStatisticsWidget::selectionChanged()
{
    if (m_selection->count() != 1)
        return;

    KoShape *shape = m_selection->firstSelectedShape();
    if (!shape) return;
    KWFrame *frame = dynamic_cast<KWFrame*>(shape->applicationData());
    if (!frame) return; // you can have embedded shapes selected, in that case it surely is no text frameset.
    KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
    if (fs) {
        if (m_textDocument)
            disconnect(m_textDocument, SIGNAL(contentsChanged()), m_timer, SLOT(start()));
        m_textDocument = fs->document();
    }
}

int KWStatisticsWidget::countCJKChars(const QString &text)
{
    int count = 0;

    QString::const_iterator it;
    for (it = text.constBegin(); it != text.constEnd(); ++it) {
        QChar qChar = *it;
        /*
         * CJK punctuations: 0x3000 - 0x303F (but I believe we shouldn't include this in the statistics)
         * Hiragana: 0x3040 - 0x309F
         * Katakana: 0x30A0 - 0x30FF
         * CJK Unified Ideographs: 4E00 - 9FFF (Chinese Traditional & Simplified, Kanji and Hanja
         * Hangul: 0xAC00 - 0xD7AF
         */
        if ((qChar >= 0x3040 && qChar <= 0x309F)
                || (qChar >= 0x30A0 && qChar <= 0x30FF)
                || (qChar >= 0x4E00 && qChar <= 0x9FFF)
                || (qChar >= 0xAC00 && qChar <= 0xD7AF)) {
            count++;
        }
    }

    return count;
}

// ----------------------------------------------------------------

void KWStatisticsWidget::wordsDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KGlobal::config()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_wordsLabel->show();
        m_countWords->show();
        cfgGroup.writeEntry("WordsVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_wordsLabel->hide();
        m_countWords->hide();
        cfgGroup.writeEntry("WordsVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::sentencesDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KGlobal::config()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_sentencesLabel->show();
        m_countSentences->show();
        cfgGroup.writeEntry("SentencesVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_sentencesLabel->hide();
        m_countSentences->hide();
        cfgGroup.writeEntry("SentencesVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::linesDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KGlobal::config()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_linesLabel->show();
        m_countLines->show();
        cfgGroup.writeEntry("LinesVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_linesLabel->hide();
        m_countLines->hide();
        cfgGroup.writeEntry("LinesVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::syllablesDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KGlobal::config()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_syllablesLabel->show();
        m_countSyllables->show();
        cfgGroup.writeEntry("SyllablesVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_syllablesLabel->hide();
        m_countSyllables->hide();
        cfgGroup.writeEntry("SyllablesVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::charspaceDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KGlobal::config()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_spacesLabel->show();
        m_countSpaces->show();
        cfgGroup.writeEntry("CharspacesVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_spacesLabel->hide();
        m_countSpaces->hide();
        cfgGroup.writeEntry("CharspacesVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::charnospaceDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KGlobal::config()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_nospacesLabel->show();
        m_countNospaces->show();
        cfgGroup.writeEntry("CharnospacesVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_nospacesLabel->hide();
        m_countNospaces->hide();
        cfgGroup.writeEntry("CharnospacesVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::eastDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KGlobal::config()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_cjkcharsLabel->show();
        m_countCjkchars->show();
        cfgGroup.writeEntry("EastAsianCharactersVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_cjkcharsLabel->hide();
        m_countCjkchars->hide();
        cfgGroup.writeEntry("EastAsianCharactersVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::fleschDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KGlobal::config()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_fleschLabel->show();
        m_countFlesch->show();
        cfgGroup.writeEntry("FleschVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_fleschLabel->hide();
        m_countFlesch->hide();
        cfgGroup.writeEntry("FleschVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}
