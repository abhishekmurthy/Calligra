/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kpresenter_view_h
#define kpresenter_view_h

#include <qguardedptr.h>
#include <qtimer.h>

#include <koView.h>
#include <kpresenter_doc.h>
#include <global.h>
#include <koRuler.h>

class QPopupMenu;
class DCOPObject;
class KPresenterView;
class BackDia;
class KoRuler;
class QScrollBar;
class AFChoose;
class StyleDia;
class PgConfDia;
class RotateDia;
class ShadowDia;
class KPPresStructView;
class ConfPieDia;
class ConfRectDia;
class ConfPolygonDia;
class QToolButton;
class SideBar;
class NoteBar;

class KAction;
class KToggleAction;
class TKSelectColorAction;
class KoPartSelectAction;

class KoCharSelectDia;
class KoTextFormat;

class KFontSizeAction;
class KColorAction;
class KSelectAction;
class KFontAction;
class KSpell;
class KoParagCounter;
class KActionMenu;
class KoSearchContext;
class KPrFindReplace;
class KPrCanvas;


class PageBase : public QWidget
{
public:
    PageBase( QWidget *parent, KPresenterView *v ) : QWidget( parent ), view( v ) {}
    void resizeEvent( QResizeEvent *e );

private:
    KPresenterView *view;

};


/*****************************************************************/
/* class KPresenterView						 */
/*****************************************************************/
class KPresenterView : public KoView
{
    friend class PageBase;
    Q_OBJECT

public:

    // ------ C++ ------
    // constructor - destructor
    KPresenterView( KPresenterDoc* _doc, QWidget *_parent = 0, const char *_name = 0 );
    ~KPresenterView();

    void initGui();
    virtual DCOPObject* dcopObject();

    // for dcop interface
    int getCurrentPresPage();
    int getCurrentPresStep();
    int getPresStepsOfPage();
    int getNumPresPages();
    bool gotoPresPage( int pg );
    float getCurrentFaktor();

    virtual void setupPrinter( KPrinter &printer );
    virtual void print( KPrinter &printer );

    KoZoomHandler* zoomHandler() const { return m_zoomHandler; }

    void showFormat( const KoTextFormat &format );

signals:
    void currentPageChanged( int );


public slots:
    // edit menu
    void editCut();
    void editCopy();
    void editPaste();
    void editDelete();
    void editSelectAll();
    void editCopyPage();
    void editDuplicatePage();
    void editDelPage();
    void editHeaderFooter();
    void editFind();
    void editReplace();

    // view menu
    void viewShowSideBar();
    void viewShowNoteBar();

    // insert menu
    void insertPage();
    void insertPicture();
    void insertPicture(const QString &file);
    void insertClipart();
    void insertSpecialChar();

    // tools menu
    void toolsMouse();
    void toolsLine();
    void toolsRectangle();
    void toolsCircleOrEllipse();
    void toolsPie();
    void toolsText();
    void toolsAutoform();
    void toolsDiagramm();
    void toolsTable();
    void toolsFormula();
    void toolsObject();
    void toolsFreehand();
    void toolsPolyline();
    void toolsQuadricBezierCurve();
    void toolsCubicBezierCurve();
    void toolsConvexOrConcavePolygon();

    // extra menu
    void extraPenBrush();
    void extraConfigPie();
    void extraConfigRect();
    void extraConfigPolygon();
    void extraRaise();
    void extraLower();
    void extraRotate();
    void extraShadow();
    //    void extraAlignObj();
    void extraBackground();
    void extraLayout();
    void extraConfigure();
    void extraLineBegin();
    void extraLineEnd();
    void extraWebPres();
    void extraCreateTemplate();
    void extraDefaultTemplate();
    void extraGroup();
    void extraUnGroup();
    void extraPenStyle();
    void extraPenWidth();

    void extraAlignObjLeft();
    void extraAlignObjCenterH();
    void extraAlignObjRight();
    void extraAlignObjTop();
    void extraAlignObjCenterV();
    void extraAlignObjBottom();

    void extraAlignObjs();

    // screen menu
    void screenConfigPages();
    void screenPresStructView();
    void screenAssignEffect();
    void screenStart();
    void screenViewPage();
    void screenStop();
    void screenPause();
    void screenFirst();
    void screenPrev();
    void screenNext();
    void screenLast();
    void screenSkip();
    void screenPenColor();
    void screenPenWidth( const QString &w );

    // text toolbar
    void sizeSelected();
    void fontSelected();
    void textBold();
    void textItalic();
    void textUnderline();
    void textStrikeOut();
    void textColor();
    void textAlignLeft();
    void textAlignCenter();
    void textAlignRight();
    void textAlignBlock();
    void mtextFont();
    void textEnumList();
    void textUnsortList();
    void textDepthPlus();
    void textDepthMinus();
    void textSettings();
    void textContentsToHeight();
    void textObjectToContents();
    void textInsertPageNum();
    void textDefaultFormat();

    // color bar
    void penChosen();
    void brushChosen();

    void skipToPage( int _num );
    void nextPage();
    void prevPage();

    /**
     * Update a given item in the sidebar
     */
    void updateSideBarItem( int pagenr );

    //statusbar updates
    void updatePageInfo();
    void updateObjectStatusBarItem();
    void pageNumChanged();

    void objectSelectedChanged();

    void extraChangeClip();
    void renamePageTitle();

    void picViewOrig640x480();
    void picViewOrig800x600();
    void picViewOrig1024x768();
    void picViewOrig1280x1024();
    void picViewOrig1600x1200();

    void chPic();

    void textSubScript();
    void textSuperScript();

    void slotSpecialChar(QChar , const QString &);
    void slotSpecialCharDlgClosed();

    void insertLink();

    void slotSoftHyphen();

    void extraAutoFormat();
    void extraSpelling();


    void spellCheckerReady();
    void spellCheckerMisspelling( QString, QStringList*, unsigned );
    void spellCheckerCorrected( QString, QString, unsigned );
    void spellCheckerDone( const QString & );
    void spellCheckerFinished( );
    void startKSpell();

    void alignChanged( int );

    void formatParagraph();
    void changeNbOfRecentFiles(int _nb);

    void insertVariable();

    void insertCustomVariable();
    void insertNewCustomVariable();
    void refreshCustomMenu();

    void editCustomVars ();
    void openLink();

    void increaseFontSize();
    void decreaseFontSize();

    void tabListChanged( const KoTabulatorList & tabList );

    void newLeftIndent( double _leftIndent);
    void newFirstIndent( double _firstIndent);
    void newRightIndent( double _rightIndent);
    void slotUpdateRuler();

    void slotHRulerDoubleClicked( double ptpos );
    void slotHRulerDoubleClicked();

    void changeCaseOfText();

public:
    // create GUI
    virtual void createGUI();

    // get - set offsets
    int getDiffX() const { return xOffset; }
    int getDiffY() const { return yOffset; }
    void setDiffX( int _x ) {xOffset = _x; }
    void setDiffY( int _y ) {yOffset = _y; }

    // get current pagenum, 1-based
    unsigned int getCurrPgNum() const;

    // return pointer to document
    class KPresenterDoc *kPresenterDoc() const {return m_pKPresenterDoc; }

    // repaint page
    void repaint( bool );
    void repaint( unsigned int, unsigned int, unsigned int, unsigned int, bool );
    void repaint( QRect, bool );

    // properties
    void changePicture( const QString & );
    void changeClipart( const QString & );

    KPrCanvas* getCanvas() { return m_canvas;}

    void setRulerMouseShow( bool _show );
    void setRulerMousePos( int mx, int my );

    // set scrollbar ranges
    void setRanges();

    KoRuler *getHRuler() { return h_ruler; }
    KoRuler *getVRuler() { return v_ruler; }
    KoTabChooser *getTabChooser() { return tabChooser; }
    QScrollBar *getHScrollBar() { return horz; }
    QScrollBar *getVScrollBar() { return vert; }

    void makeRectVisible( QRect _rect );

    void restartPresStructView();

    PieType getPieType() { return pieType; }
    int getPieAngle() { return pieAngle; }
    int getPieLength() { return pieLength; }
    QPen getPen() { return pen; }
    QBrush getBrush() { return brush; }
    LineEnd getLineBegin() {return lineBegin; }
    LineEnd getLineEnd() {return lineEnd; }
    QColor getGColor1() {return gColor1; }
    QColor getGColor2() {return gColor2; }
    BCType getGType() {return gType; }
    FillType getFillType() {return fillType; }
    bool getGUnbalanced() { return gUnbalanced; }
    int getGXFactor() { return gXFactor; }
    int getGYFactor() { return gYFactor; }

    void setTool( ToolEditMode toolEditMode );

    int getRndX() { return rndX; }
    int getRndY() { return rndY; }

    QFont &currFont() { return tbFont; }
    QColor &currColor() { return tbColor; }

    void enableWebPres();

    /**
     * Overloaded from View
     */
    bool doubleClickActivation() const;
    /**
     * Overloaded from View
     */
    QWidget* canvas();
    /**
     * Overloaded from View
     */
    int canvasXOffset() const;
    /**
     * Overloaded from View
     */
    int canvasYOffset() const;

    /**
     * Rebuild the whole sidebar
     */
    void updateSideBar();

    void refreshPageButton();


    void openPopupMenuMenuPage( const QPoint & _point );
    void openPopupMenuTextObject( const QPoint & _point );
    void openPopupMenuPartObject( const QPoint & _point );
    void openPopupMenuRectangleObject( const QPoint & _point );
    void openPopupMenuGraphMenu(const QPoint & _point );
    void openPopupMenuPieObject( const QPoint & _point );
    void openPopupMenuClipObject(const QPoint & _point);
    void openPopupMenuSideBar(const QPoint & _point);
    void openPopupMenuPicObject(const QPoint & _point);
    void openPopupMenuPolygonObject( const QPoint &_point );

    void penColorChanged( const QPen & _pen );
    void brushColorChanged( const QBrush & _brush );

    void autoScreenPresReStartTimer();
    void autoScreenPresIntervalTimer();
    void autoScreenPresStopTimer();
    void setCurrentTimer( int _currentTimer );

    void showCounter( KoParagCounter &c );

    QPopupMenu * popupMenu( const QString& name );

    void showRulerIndent( double _leftMargin, double _firstLine, double _rightMargin );

    void reorganize();

    // For NoteBar
    NoteBar *getNoteBar() { return notebar; }

    // Used by Page to plug/unplug the datatool actions
    QPtrList<KAction>& actionList() { return m_actionList; }

    // for Polygon object
    bool getCheckConcavePolygon() { return checkConcavePolygon; }
    int getCornersValue() { return cornersValue; }
    int getSharpnessValue() { return sharpnessValue; }

protected slots:
    // dialog slots
    void backOk( bool );
    void afChooseOk( const QString & );
    void slotAfchooseCanceled();
    void styleOk();
    void pgConfOk();
    void effectOk();
    void rotateOk();
    void shadowOk();
    void psvClosed();
    void confPieOk();
    void confRectOk();
    void confPolygonOk();

    // scrolling
    void scrollH( int );
    void scrollV( int );

    // textobject
    void fontChanged( const QFont & );
    void colorChanged( const QColor &color );

    void extraLineBeginNormal();
    void extraLineBeginArrow();
    void extraLineBeginRect();
    void extraLineBeginCircle();
    void extraLineEndNormal();
    void extraLineEndArrow();
    void extraLineEndRect();
    void extraLineEndCircle();

    void extraPenStyleSolid();
    void extraPenStyleDash();
    void extraPenStyleDot();
    void extraPenStyleDashDot();
    void extraPenStyleDashDotDot();
    void extraPenStyleNoPen();
    void setExtraPenStyle( Qt::PenStyle style );

    void extraPenWidth1();
    void extraPenWidth2();
    void extraPenWidth3();
    void extraPenWidth4();
    void extraPenWidth5();
    void extraPenWidth6();
    void extraPenWidth7();
    void extraPenWidth8();
    void extraPenWidth9();
    void extraPenWidth10();
    void setExtraPenWidth( unsigned int width );

    void stopPres() {continuePres = false; }
    void newPageLayout( KoPageLayout _layout );
    void openPageLayoutDia() { extraLayout(); }
    void unitChanged( QString );

    void doAutomaticScreenPres();

    void getPageMouseWheelEvent( QWheelEvent *e );
    void updateRuler();

    void refreshAllVariable();

// end of protected slots
protected:

// ********* functions ***********

// resize event
    virtual void resizeEvent( QResizeEvent* );
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dragLeaveEvent( QDragLeaveEvent *e );
    virtual void dropEvent( QDropEvent *e );

    virtual void keyPressEvent( QKeyEvent* );

    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

// GUI
    void setupActions();
    void setupPopupMenus();
    void setupScrollbars();
    void setupRulers();

    void startScreenPres( int pgNum = -1 );

    virtual void updateReadWrite( bool readwrite );

    void addVariableActions( int type, const QStringList & texts,
                             KActionMenu * parentMenu, const QString & menuText );

    void showParagraphDialog( int initialPage = -1, double initialTabPos = 0.0 );
    void doFindReplace();

private:
// ********** variables **********

    // document
    KPresenterDoc *m_pKPresenterDoc;

    // zoom handler
    KoZoomHandler* m_zoomHandler;

    // flags
    bool continuePres, exitPres;
    bool m_screenSaverWasEnabled;

    // right button popup menus
    QPopupMenu *rb_oalign, *rb_lbegin, *rb_lend, *rb_pstyle, *rb_pwidth;
    QPtrList<KAction> m_actionList; // for the kodatatools

    // scrollbars
    QScrollBar *vert, *horz;
    int xOffset, yOffset;
    int xOffsetSaved, yOffsetSaved; // saved when going fullscreen

    // dialogs
    BackDia *backDia;
    AFChoose *afChoose;
    StyleDia *styleDia;
    PgConfDia *pgConfDia;
    RotateDia *rotateDia;
    ShadowDia *shadowDia;
    KPPresStructView *presStructView;
    ConfPieDia *confPieDia;
    ConfRectDia *confRectDia;
    ConfPolygonDia *confPolygonDia;

    // default pen and brush
    QPen pen;
    QBrush brush;
    LineEnd lineBegin;
    LineEnd lineEnd;
    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;
    PieType pieType;
    bool gUnbalanced;
    int gXFactor, gYFactor;
    int pieLength, pieAngle;
    int rndX, rndY;
    bool sticky;

    // for Convex/Concave Polygon
    bool checkConcavePolygon;
    int cornersValue;
    int sharpnessValue;

    // the page
    KPrCanvas *m_canvas;
    KoRuler *h_ruler, *v_ruler;
    KoTabChooser *tabChooser;
    // text toolbar values
    QFont tbFont;
    int tbAlign;
    QColor tbColor;
    QStringList fontList;

    bool m_bRectSelection;
    QRect m_rctRectSelection;
    QString m_strNewPart;
    bool m_bShowGUI;
    bool presStarted;
    bool allowWebPres;
    int currPg; // 0-based

    QSize oldSize;

    int screensaver_pid;

    // Statusbar items
    QLabel * m_sbPageLabel; // 'Current page number and page count' label
    QLabel * m_sbObjectLabel; // Info about selected object

    // actions
    KAction *actionEditCut;
    KAction *actionEditCustomVars;
    KAction *actionEditCopy;
    KAction *actionEditPaste;
    KAction *actionEditDelete;
    KAction *actionEditSelectAll;
    KAction *actionEditDuplicatePage;
    KAction *actionEditDelPage;
    KAction *actionEditHeaderFooter;

    KToggleAction *actionViewShowSideBar;
    KToggleAction *actionViewShowNoteBar;

    KAction *actionInsertPage;
    KAction *actionInsertPicture;
    KAction *actionInsertClipart;

    KToggleAction *actionToolsMouse;
    KToggleAction *actionToolsLine;
    KToggleAction *actionToolsRectangle;
    KToggleAction *actionToolsCircleOrEllipse;
    KToggleAction *actionToolsPie;
    KToggleAction *actionToolsText;
    KToggleAction *actionToolsAutoform;
    KToggleAction *actionToolsDiagramm;
    KToggleAction *actionToolsTable;
    KToggleAction *actionToolsFormula;
    KToggleAction *actionToolsFreehand;
    KToggleAction *actionToolsPolyline;
    KToggleAction *actionToolsQuadricBezierCurve;
    KToggleAction *actionToolsCubicBezierCurve;
    KToggleAction *actionToolsConvexOrConcavePolygon;
    KoPartSelectAction *actionToolsObject;

    KAction *actionTextFont;
    KFontSizeAction *actionTextFontSize;
    KFontAction *actionTextFontFamily;
    KToggleAction *actionTextAlignLeft;
    KToggleAction *actionTextAlignCenter;
    KToggleAction *actionTextAlignRight;
    KToggleAction *actionTextAlignBlock;
    KToggleAction *actionTextTypeEnumList;
    KToggleAction *actionTextTypeUnsortList;
    KAction *actionTextDepthPlus;
    KAction *actionTextDepthMinus;
    KAction *actionTextSettings;
    KAction *actionTextExtentCont2Height;
    KAction *actionTextExtendObj2Cont;
    KToggleAction *actionTextBold;
    KToggleAction *actionTextItalic;
    KToggleAction *actionTextUnderline;
    KToggleAction *actionFormatStrikeOut;
    KAction *actionTextInsertPageNum;

    KAction *actionExtraPenBrush;
    KAction *actionExtraConfigPie;
    KAction *actionExtraConfigRect;
    KAction *actionExtraConfigPolygon;
    KAction *actionExtraRaise;
    KAction *actionExtraLower;
    KAction *actionExtraRotate;
    KAction *actionExtraShadow;
    KAction *actionExtraAlignObjs;
    KAction *actionExtraAlignObjLeft;
    KAction *actionExtraAlignObjCenterH;
    KAction *actionExtraAlignObjRight;
    KAction *actionExtraAlignObjTop;
    KAction *actionExtraAlignObjCenterV;
    KAction *actionExtraAlignObjBottom;
    KAction *actionExtraBackground;
    KAction *actionExtraLayout;
    KAction *actionExtraConfigure;
    KAction *actionExtraWebPres;
    KAction *actionExtraCreateTemplate;
    KAction *actionExtraLineBegin;
    KAction *actionExtraLineEnd;
    KAction *actionExtraGroup;
    KAction *actionExtraUnGroup;
    KAction *actionExtraPenStyle;
    KAction *actionExtraPenWidth;

    KAction *actionScreenConfigPages;
    KAction *actionScreenPresStructView;
    KAction *actionScreenAssignEffect;
    KAction *actionScreenStart;
    KAction *actionScreenViewPage;
    KAction *actionScreenStop;
    KAction *actionScreenPause;
    KAction *actionScreenFirst;
    KAction *actionScreenPrev;
    KAction *actionScreenNext;
    KAction *actionScreenLast;
    KAction *actionScreenSkip;

    KAction *actionEditSearch;
    KAction *actionEditReplace;
    KColorAction *actionScreenPenColor;
    KSelectAction *actionScreenPenWidth;

    KAction *actionColorBar;
    KAction *actionExtraDefaultTemplate;

    TKSelectColorAction* actionBrushColor;
    TKSelectColorAction* actionPenColor;
    TKSelectColorAction* actionTextColor;


    KAction *actionResizeTextObject;
    KAction *actionExtendObjectHeight;
    KAction *actionObjectProperties;
    KAction *actionChangeClipart;

    KAction *actionRenamePage;

    KAction *actionPic640x480;
    KAction *actionPic800x600;
    KAction *actionPic1024x768;
    KAction *actionPic1280x1024;
    KAction *actionPic1600x1200;
    KAction *actionChangePic;
    KAction *actionExtraSpellCheck;
    KAction *actionFormatDefault;


    DCOPObject *dcop;

    QToolButton *pgNext, *pgPrev;
    SideBar *sidebar;
    NoteBar *notebar;
    QSplitter *splitter;
    PageBase *pageBase;

    KToggleAction *actionFormatSuper;
    KToggleAction *actionFormatSub;

    KAction* actionInsertSpecialChar;
    KAction *actionInsertLink;
    KAction *actionFormatParag;
    KAction *actionOpenLink;
    KAction *actionIncreaseFontSize;
    KAction *actionDecreaseFontSize;
    KAction *actionChangeCase;
    KAction *actionRefreshAllVariable;
    QTimer automaticScreenPresTimer;
    QTime automaticScreenPresTime;
    int automaticScreenPresWaitTime;
    bool automaticScreenPresFirstTimer;
    int currentTimer;

    KoCharSelectDia *m_specialCharDlg;


    QStringList m_ignoreWord;
    // Spell-checking
    struct {
	KSpell *kspell;
	int spellCurrTextObjNum;
	QPtrList<KPTextObject> textObject;
	QStringList ignoreWord;
	KMacroCommand * macroCmdSpellCheck;
     } m_spell;

    KActionMenu *actionInsertVariable;
    KActionMenu *actionInsertCustom;

    struct VariableDef {
        int type;
        int subtype;
    };
    typedef QMap<KAction *, VariableDef> VariableDefMap;
    VariableDefMap m_variableDefMap;

    KoSearchContext *m_searchEntry, *m_replaceEntry;
    KPrFindReplace *m_findReplace;

};

#endif
