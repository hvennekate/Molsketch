/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2009 Tim Vandermeersch                                  *
 *   Copyright (C) 2009 by Nicola Zonta                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QClipboard>
#include <QApplication>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QKeyEvent>
#include <QUndoStack>
#include <QMenu>
#include <QProcess>
#include <QDir>
#include <QLibrary>
#include <QMessageBox>
#include <QSvgGenerator>
#include <QPushButton>
#include <QBuffer>
#if QT_VERSION < 0x050000
#include <QtMath>
#include <QDesktopServices>
#else
#include <QtCore/qmath.h>
#include <QStandardPaths>
#endif
#include <QDebug>
#include <QDockWidget>
#include <QLabel>
#include <QMainWindow>
#include <QPair>
#include <QSettings>
#include <QVBoxLayout>

#include "molscene.h"

#include "element.h"
#include "atom.h"
#include "bond.h"
#include "residue.h"

#include "molecule.h"
#include "commands.h"
#include "smilesitem.h"
#include "mimemolecule.h"
#include "TextInputItem.h"
#include "math2d.h"
#include "obabeliface.h"
#include "grid.h"
#include "molview.h"

#include "arrow.h"
#include "actions/abstractitemaction.h"
#include "actions/genericaction.h"

#include "frame.h"

#ifdef QT_STATIC_BUILD
inline void initToolBarIcons() { Q_INIT_RESOURCE(toolicons); }
#endif

namespace Molsketch {

#ifdef QT_STATIC_BUILD
  void initToolBarIconsMsk() { initToolBarIcons(); }
#endif

  const QString MolScene::mouseWheelForCyclingTools = "mouse-wheel-cycle-tools";

  struct MolScene::privateData
  {
    QGraphicsRectItem *selectionRectangle;
    TextInputItem *inputItem;
    grid *Grid;
    MolScene *scene;
    QDockWidget *propertiesDock;
    QLabel *propertiesHelpLabel;

    QAction *bondLengthAction,
    *bondWidthAction,
    *arrowWidthAction,
    *frameLinewidthAction,
    *bondAngleAction,
    *atomSizeAction,
    *atomFontAction,
    *hydrogenVisibleAction,
    *carbonVisibleAction,
    *lonePairsVisibleAction,
    *autoAddHydrogenAction,
    *electronSystemsVisibleAction,
    *chargeVisibleAction,
    *defaultColorAction;

    graphicsItem* dragItem;

    QMap<QAction*, QPair<void (MolScene::*)(const bool&), bool (MolScene::*)() const> > booleanActions;
    void attachDockWidgetToMainWindow(MolScene* scene)
    {
      if (scene)
      {
        QMainWindow* mainWindow = qobject_cast<QMainWindow*>(scene->parent());
        if (mainWindow)
          mainWindow->addDockWidget(Qt::LeftDockWidgetArea, propertiesDock);
      }
    }

    privateData(MolScene* scene)
      : selectionRectangle(new QGraphicsRectItem),
        inputItem(new TextInputItem),
        Grid(new grid),
        scene(scene),
        propertiesDock(new QDockWidget(tr("Properties"))),
        propertiesHelpLabel(new QLabel(tr("Select single item to edit properties"), propertiesDock)),
        bondLengthAction(new QAction(tr("Bond length..."), scene)),
        bondWidthAction(new QAction(tr("Bond linewidth..."), scene)),
        arrowWidthAction(new QAction(tr("Arrow width..."), scene)),
        frameLinewidthAction(new QAction(tr("Bracket linewidth..."), scene)),
        bondAngleAction(new QAction(tr("Bond snap angle..."), scene)),
        atomSizeAction(new QAction(tr("Atom size..."), scene)),
        atomFontAction(new QAction(tr("Atom font..."), scene)),
        hydrogenVisibleAction(new QAction(tr("Hydrogens visible"), scene)),
        carbonVisibleAction(new QAction(tr("Carbons visible"), scene)),
        lonePairsVisibleAction(new QAction(tr("Lone pairs visible"), scene)),
        autoAddHydrogenAction(new QAction(tr("Auto add hydrogens"), scene)),
        electronSystemsVisibleAction(new QAction(tr("Electron systems visible"), scene)),
        chargeVisibleAction(new QAction(tr("Charges visible"), scene)),
        defaultColorAction(new QAction(tr("Default color..."), scene)),
        dragItem(0)
    {
      selectionRectangle->setPen(QPen(Qt::blue,0,Qt::DashLine));
      selectionRectangle->setZValue(INFINITY);
#define REGISTER_ACTION(CAPLETTER, LOWERLETTER, PROPNAME) booleanActions[LOWERLETTER##PROPNAME##Action] = qMakePair(&MolScene::set##CAPLETTER##PROPNAME, &MolScene::LOWERLETTER##PROPNAME);
      REGISTER_ACTION(H,h,ydrogenVisible)
      REGISTER_ACTION(C,c,arbonVisible)
      REGISTER_ACTION(L,l,onePairsVisible)
      REGISTER_ACTION(A,a,utoAddHydrogen)
      REGISTER_ACTION(E,e,lectronSystemsVisible)
      REGISTER_ACTION(C,c,hargeVisible)
      foreach(QAction* booleanAction, booleanActions.keys())
      {
        booleanAction->setCheckable(true);
        booleanAction->setChecked((scene->*booleanActions[booleanAction].second)());
        connect(booleanAction, SIGNAL(toggled(bool)), scene, SLOT(booleanPropertyChanged(bool)));
        connect(booleanAction, SIGNAL(toggled(bool)), scene, SLOT(updateAll()));
      }

      propertiesHelpLabel->setAlignment(Qt::AlignTop);
      propertiesHelpLabel->setDisabled(true);
      propertiesDock->setWidget(propertiesHelpLabel);
      attachDockWidgetToMainWindow(scene);
    }

    QMenu* contextSubMenu()
    {
      QMenu* menu = new QMenu(tr("Scene properties"));
      menu->addActions(booleanActions.keys());
      return menu;
    }

    ~privateData()
    {
//      if (inputItem && !inputItem->scene()) // TODO compare with this scene
//        delete inputItem; // TODO should clean up this item...
//      delete selectionRectangle; // TODO why?
      propertiesDock->setWidget(propertiesHelpLabel);
      delete propertiesDock;
      if (!Grid->scene()) delete Grid;
    }

    bool gridOn()const { return Grid->scene(); }
  };

  using namespace Commands;


  //////////////////////////////////////////////////////////////////////////////
  //
  // Constructor & destructor
  //
  //////////////////////////////////////////////////////////////////////////////

  MolScene::MolScene(QObject* parent)
    : QGraphicsScene(parent),
      d(new privateData(this))
  {

    //Initializing properties
    m_editMode = MolScene::DrawMode;
    m_renderMode = RenderLabels;

    // Prepare undo m_stack
    m_stack = new QUndoStack(this);
    connect(m_stack, SIGNAL(indexChanged(int)), this, SIGNAL(documentChange()));
    connect(m_stack, SIGNAL(indexChanged(int)), this, SLOT(update()));
    connect(m_stack, SIGNAL(indexChanged(int)), this, SLOT(updateAll())) ;
    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionSlot()));

    // Set initial size
    QRectF sizerect(-5000,-5000,10000,10000);
    setSceneRect(sizerect);
//    f->setCoordinates(QPointF(0,0), QPointF(100,100));
//    f->setParentItem(m);
//    f->setFrameString("(r .5,r.5)+(-10,0)-+( 10,0)-(r .5,r-.5)-+(-10,0)"
//                        "(r-.5,r.5)+( 10,0)-+(-10,0)-(r-.5,r-.5)-+( 10,0)"); // brackets
//    f->setFrameString("(r.5,r-.5)+(-20,0)-+(20,0)-+(0,20)"); // angle
//    f->setFrameString("(r.5,r-.5)+(-10,0).+(10,0).+(0,10)"
//                      "$(r.5,r0)-+(0,-10).+(0,10).+(5,0)"
//                      ".+(-5,0).+(0,10)$(r.5,r.5)-+(0,-10)"
//                      ".+(0,10).+(-10,0)"
//                      ); // curly bracket

//    f->setFrameString("(r.5,r.5)-(r.5,r-.5)-(r-.5,r-.5)-(r-.5,r.5)-(r.5,r.5)"); // full frame
  }

  MolScene::~MolScene()
  {
    clear();
  }

  void MolScene::addResidue (QPointF pos, QString name)
  {
#if QT_VERSION < 0x050000
        m_stack ->push (new AddResidue (new Residue (pos, name, 0, this)));
#else
        Residue* newResidue = new Residue(pos, name, 0) ;
        addItem(newResidue) ;
        m_stack->push(new AddResidue(newResidue)) ;
#endif
  }

  QString MolScene::mimeType()
  {
    return "molecule/molsketch";
  }
  // Commands

  QColor MolScene::color() const
  {
    return defaultColor();
  }

  void MolScene::setColor (const QColor& c)
  {
    foreach (QGraphicsItem* item, selectedItems()) {
      if (item->type() == Atom::Type) {
        dynamic_cast<Atom*>(item) ->setColor(c);
      }
      else if (item->type() == Residue::Type) {
        dynamic_cast<Residue*>(item) ->setColor(c);
      }
    }
    foreach (QGraphicsItem* item, items()) {
      if (item->type() == Bond::Type) {
        Bond *b = dynamic_cast<Bond*>(item);
        if (b-> beginAtom() ->isSelected () && b->endAtom() ->isSelected()) b->setColor(c);
      }
    }
    if (selectedItems().isEmpty())
      setDefaultColor(c);
  }

  void MolScene::alignToGrid()
  {
    setGrid(true);
        m_stack->beginMacro(tr("aligning to grid"));
        foreach(QGraphicsItem* item,items())
          if (item->type() == Molecule::Type)
                m_stack->push(MoveItem::relative(item,toGrid(item->scenePos()) - item->scenePos()));
        m_stack->endMacro();
        update();
  }

  void MolScene::setEditMode(int mode)
  {
    // TODO
//        // Reset moveflag (movebug)
//        foreach(QGraphicsItem* item, items())
//          item->setFlag(QGraphicsItem::ItemIsMovable, false);

//	// enable moving for all Molecule and atom items
//	foreach(QGraphicsItem* item, items())
//	  if (item->type() == Molecule::Type || item->type() == Atom::Type)
//		item->setFlag(QGraphicsItem::ItemIsSelectable,mode == MolScene::MoveMode);



        // Set the new edit mode and signal other components
        m_editMode = mode;
        emit editModeChange( mode );
  }

  void MolScene::cut()
  {
        /* TODO Using the desktop clipboard*/
        // Check if something is selected
        if (selectedItems().isEmpty()) return;

        // Then do a copy
        copy();

        // Finally delete the selected items
        m_stack->beginMacro(tr("cutting items"));
        foreach (QGraphicsItem* item, selectedItems())
          if (item->type() == Molecule::Type) m_stack->push(new DelItem(item));
        m_stack->endMacro();
  }

  void MolScene::copy()
  {
        // Check if something is selected
        if (selectedItems().isEmpty()) return;

        /* TODO Using the desktop clipboard */
        // Access the clipboard
        QClipboard* clipboard = qApp->clipboard();

        // Calculate total boundingrect
        QRectF totalRect;
        foreach(QGraphicsItem* item, selectedItems())
        {
          QRectF itemRect = item->boundingRect();
          itemRect.translate(item->scenePos());
          totalRect |= itemRect;
        }
        // Add to internal clipboard
        foreach(QGraphicsItem* item, m_clipItems) delete item;
        m_clipItems.clear();
        foreach(QGraphicsItem* item, selectedItems())
          if (item->type() == Molecule::Type)
                m_clipItems.append(new Molecule(dynamic_cast<Molecule*>(item)));

        // Clear selection
        QList<QGraphicsItem*> selList(selectedItems());
        clearSelection();

        // Choose the datatype
        //   clipboard->setText("Test");
        clipboard->setImage(renderImage(totalRect));
        //   clipboard->mimeData( );

        // Restore selection
        foreach(QGraphicsItem* item, selList) item->setSelected(true);

        // Emit paste available signal
        emit pasteAvailable(!m_clipItems.isEmpty());
  }

  void MolScene::paste()
  {
        // Access the clipboard
        //   QClipboard* clipboard = qApp->clipboard();
        /* TODO Using the system clipboard*/

        // Paste all items on the internal clipboard
        m_stack->beginMacro(tr("pasting items"));
        foreach(Molecule* item, m_clipItems) m_stack->push(new AddItem(new Molecule(item),this));
        m_stack->endMacro();
  }

  void MolScene::convertImage()
  {
        QClipboard* clipboard = qApp->clipboard();
        QImage img = clipboard->image();
        if (img.isNull()) return ;

        QLibrary obabeliface("obabeliface" QTVERSIONSUFFIX);
        obabeliface.load() ;
        callOsraFunctionPointer callOsraPtr = (callOsraFunctionPointer) obabeliface.resolve("call_osra") ;
        if (!callOsraPtr)
        {
          QMessageBox::critical(0, tr("Error importing image"), tr("OpenBabel support unavailable.")) ;
          return ;
        }
#if QT_VERSION < 0x050000
        QString tmpimg = QDesktopServices::storageLocation(QDesktopServices::TempLocation) + QDir::separator() + "osra.png";
#else
        QString tmpimg = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QDir::separator() + "osra.png";
#endif
        img.save(tmpimg, "PNG", 100);
        Molecule* mol = callOsraPtr(tmpimg);
        if (mol)
        {
          m_stack->beginMacro(tr("converting image using OSRA"));
          m_stack->push(new AddItem(new Molecule(mol), this));
          m_stack->endMacro();
        }
        else
          QMessageBox::critical(0, tr("Error"), tr("OSRA conversion failed. Is OSRA installed?")) ;
        QFile::remove(tmpimg);
  }

#ifdef QT_DEBUG
  void MolScene::debugScene()
  {
    qDebug() << "================= Scene debug =================";
    foreach(QGraphicsItem *item, items())
    {
      qDebug() << "Item:" << item
               << "Type:" << item->type()
               << "Pos:"  << item->pos()
               << "Scene Pos:" << item->scenePos()
               << "Bounds:" << item->boundingRect()
               << "Children:" << item->childItems() ;
    }
    qDebug() << "============== Undo stack debug ===============";
    qDebug() << "position:" << stack()->index();
    for (int i = 0 ; i < stack()->count() ; ++i)
    {
      const QUndoCommand* command = stack()->command(i);
      qDebug() << i << command << command->id() << command->text();
    }
    qDebug() << "===============================================";
  }
#endif

  void MolScene::clear()
  {
    clearSelection();
    QGraphicsScene::clear();
    m_stack->clear();
    delete d;
    d = new privateData(this);
  }

  QImage MolScene::renderMolToImage (Molecule *mol)
  {
                QRectF rect = mol ->boundingRect();
                QImage image(int(rect.width()),int(rect.height()),QImage::Format_RGB32);
                image.fill(QColor("white").rgb());

                // Creating and setting the painter
                QPainter painter(&image);
                painter.setRenderHint(QPainter::Antialiasing);

                // Rendering in the image and saving to file
                render(&painter,QRectF(0,0,rect.width(),rect.height()),QRectF (mol ->mapToScene (rect.topLeft ()), mol ->mapToScene (rect.bottomRight ())));
                return image;
  }

  QByteArray MolScene::toSvg()
  {
    QList<QGraphicsItem*> selection(selectedItems());
    clearSelection();
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QBuffer::WriteOnly);
    QSvgGenerator svgGenerator;
    svgGenerator.setTitle(tr("MolsKetch Drawing"));
    QRectF bounds(itemsBoundingRect());
    svgGenerator.setSize(bounds.size().toSize()); // TODO round up
    svgGenerator.setViewBox(bounds);
    svgGenerator.setOutputDevice(&buffer);
    QPainter painter;
    painter.begin(&svgGenerator);
    render(&painter, bounds, bounds);
    painter.end();
    buffer.close();
    // TODO reselect items
    return ba;
  }


  QImage MolScene::renderImage(const QRectF &rect)
  {
        // Creating an image
        QImage image(int(rect.width()),int(rect.height()),QImage::Format_RGB32);
        image.fill(QColor("white").rgb());

        // Creating and setting the painter
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing);

        // Rendering in the image and saving to file
        render(&painter,QRectF(0,0,rect.width(),rect.height()),rect);

        return image;
  }

  void MolScene::addMolecule(Molecule* mol)
  {
        Q_CHECK_PTR(mol);
        if (!mol) return;
        m_stack->beginMacro(tr("add molecule"));
        m_stack->push(new AddItem(mol,this));
        if (mol->canSplit()) m_stack->push(new SplitMol(mol));
        m_stack->endMacro();
  }

  void MolScene::selectAll()
  {
        // Switch to move mode to make selection posible
        setEditMode(MolScene::MoveMode);

        // Clear any previous selection
        clearSelection();

        // Mark all atoms as selected
        foreach (QGraphicsItem* item, items())
        {
          if (item->type() == Molecule::Type || item->type() == Arrow::Type)
                item->setSelected(true);
        }
  }


  void MolScene::setHoverRect( QGraphicsItem* item )
  {
        if (item)
        {
          m_hoverRect->setPath(item->shape());
          m_hoverRect->setPos(item->scenePos());
          addItem(m_hoverRect);
        }
        else
          removeItem(m_hoverRect);
  }

  int MolScene::editMode() const
  {
        return m_editMode;
  }

  MolScene::RenderMode MolScene::renderMode() const
  {
        return m_renderMode;
  }

  void MolScene::setRenderMode(MolScene::RenderMode mode)
  {
        m_renderMode = mode;
  }

  QPointF MolScene::toGrid(const QPointF &position) // TODO obsolete
  {
        QPointF p = position;
        int factor = 40;
        p.rx() = floor(p.x() / factor) * factor;
        p.ry() = floor(p.y() / factor) * factor;

        return p;
  }

  QPointF MolScene::snapToGrid(const QPointF &point, bool force)
  {
    if (!d->gridOn() && !force) return point;
    return d->Grid->alignPoint(point);
  }

  bool MolScene::snappingToGrid() const
  {
    return d->gridOn();
  }

  bool MolScene::cyclingByMouseWheelEnaled() const
  {
    QSettings settings;
    QVariant mouseWheelForTools = settings.value(mouseWheelForCyclingTools);
    if (mouseWheelForTools.isValid())
        return mouseWheelForTools.toBool();
    QMessageBox promptForMouseWheelUsage;
    promptForMouseWheelUsage.setWindowTitle(tr("Mouse wheel configuration"));
    promptForMouseWheelUsage.setText(tr("Mouse wheel use has not been configured."
                                        "Should the wheel be used to zoom, "
                                        "or to cycle tool settings?"));
    QAbstractButton *zoomButton = promptForMouseWheelUsage.addButton(tr("Use to zoom"), QMessageBox::YesRole);
    QAbstractButton *cycleButton = promptForMouseWheelUsage.addButton(tr("Use to cycle tool options"), QMessageBox::NoRole);
    promptForMouseWheelUsage.exec();
    if (promptForMouseWheelUsage.clickedButton() == zoomButton)
    {
      settings.setValue(mouseWheelForCyclingTools, false);
      return false;
    }
    if (promptForMouseWheelUsage.clickedButton() == cycleButton)
    {
      settings.setValue(mouseWheelForCyclingTools, true);
      return true;
    }
    return false;
  }

  void MolScene::setGrid(bool on)
  {
    if (on) addItem(d->Grid);
    else removeItem(d->Grid);
  }

  abstractXmlObject *MolScene::produceChild(const QString &childName, const QString &type)
  {
    graphicsItem *object = 0 ;
    if ("frame" == childName) object = new Frame;
    if (childName == "molecule") // TODO move those names to their classes.
      object = new Molecule;
    if (childName == "arrow")
      object = new Arrow;
    if (childName == "object")
    {
      if (type == "ReactionArrow") object = new Arrow ;
      if (type == "MechanismArrow") object = new Arrow ;
    }
    if (childName == "plugin")
      object = ItemPluginFactory::createInstance(type);
    if (object) addItem(object) ;
    return object ;
  }

  QList<const abstractXmlObject *> MolScene::children() const
  {
        QList<const abstractXmlObject*> childrenList ;
        foreach(QGraphicsItem* item, items())
        {
          graphicsItem *gItem = dynamic_cast<graphicsItem*>(item);
          if (gItem && gItem->parentItem()) continue;
          childrenList << dynamic_cast<const abstractXmlObject*>(item) ;
        }
        return childrenList ;
  }

  void MolScene::readAttributes(const QXmlStreamAttributes &attributes)
  {
    foreach(const QXmlStreamAttribute& attribute, attributes)
      setProperty(attribute.name().toLatin1(), attribute.value().toString());
    clear();
  }

  QXmlStreamAttributes MolScene::xmlAttributes() const
  {
    QXmlStreamAttributes attributes;
    foreach (const QByteArray& name, dynamicPropertyNames()) // TODO handle int, double, bool appropriately
      attributes.append(name, property(name).toString());
    return attributes;
  }

  Molecule* MolScene::moleculeAt(const QPointF &pos)
  {
        // Check if there is a molecule at this position
        foreach(QGraphicsItem* item,items(pos))
          if (item->type() == Molecule::Type) return dynamic_cast<Molecule*>(item);

        // Else return NULL
        return 0;

  }

  TextInputItem *MolScene::inputItem()
  {
    return d->inputItem;
  }

  QMenu *MolScene::sceneMenu() const
  {
    return d->contextSubMenu();
  }

  QList<Atom *> MolScene::atoms() const
  {
    QList<Atom*> result;
    foreach(auto item, items())
    {
      Atom* atom = dynamic_cast<Atom*>(item);
      if (atom) result << atom;
    }
    return result;
  }

  void MolScene::selectionSlot()
  {
    foreach(AbstractItemAction* itemAction, findChildren<AbstractItemAction*>())
      itemAction->setItems(selectedItems());
    graphicsItem* currentItem = 0; // TODO check if focusItem is what we actually want here
    if (selectedItems().size() == 1) currentItem = dynamic_cast<graphicsItem*>(selectedItems().first());
    if (currentItem) d->propertiesDock->setWidget(currentItem->getPropertiesWidget());
    else {
      QWidget* oldWidget = d->propertiesDock->widget();
      d->propertiesDock->setWidget(d->propertiesHelpLabel);
      if (oldWidget != d->propertiesHelpLabel) delete oldWidget;
    }
  }

  void MolScene::booleanPropertyChanged(bool newValue)
  {
    QAction* action = dynamic_cast<QAction*>(sender());
    if (!d->booleanActions.contains(action)) return;
    (this->*d->booleanActions.value(action).first)(newValue);
  }

  Atom* MolScene::atomAt(const QPointF &pos)
  {
    foreach(Atom* atom, atoms())
      if (atom->scenePos() == pos)
        return atom;
    return 0;
  }

  Atom* MolScene::atomNear(const QPointF &pos, qreal tolerance)
  {
    Atom* pickedAtom = atomAt(pos);
    if (pickedAtom) return pickedAtom;
    foreach(auto atom, atoms())
    {
      qreal newDistance = QLineF(atom->scenePos(), pos).length();
      if (newDistance < tolerance)
      {
          pickedAtom = atom;
          tolerance = newDistance;
      }
    }
    return pickedAtom;
  }

  Bond* MolScene::bondAt(const QPointF &pos)
  {
        // Check if there is a bond at this position
        foreach( QGraphicsItem* item,items(pos))
          if (item->type() == Bond::Type) return dynamic_cast<Bond*>(item);

        // Else return NULL
        return 0;
  }

  // Event handlers

  bool MolScene::event(QEvent* event)
  {
        // Execute default behaivior
        bool accepted = QGraphicsScene::event(event);

        // Check whether copying is available
        if ((event->type() == QEvent::GraphicsSceneMouseRelease) || (event->type() == QEvent::KeyRelease))
          emit copyAvailable(!selectedItems().isEmpty());

        // Execute default behavior
        return accepted;
  }


  void MolScene::updateAll()
  {
    invalidate() ;
    update() ;
  }

  void MolScene::keyPressEvent(QKeyEvent* keyEvent)
  {
//          if ( !m_inputTextItem ->hasFocus ()) {
//        // Declare item
//        QGraphicsItem* item;
//        Atom* atom;
//        //   Bond* bond;
//        //   Molecule* mol;
//        QSet<Molecule*> molSet;

//	switch (keyEvent->key())
//	{
//	  case Qt::Key_Delete:
//		m_stack->beginMacro(tr("removing item(s)"));
//		// First delete all selected molecules
//		foreach (item, selectedItems())
//		  if (item->type() == Molecule::Type)
//		  {
//			m_stack->push(new DelItem(item));
//		  }
//		//       // Then delete
//		//       foreach (item, selectedItems())
//		//         if (item->type() == Bond::Type)
//		//         {
//		//           bond = dynamic_cast<Bond*>(item);
//		//           mol = bond->molecule();
//		//           m_stack->push(new DelBond(bond));
//		//           if (mol->canSplit()) m_stack->push(new SplitMol(mol));
//		//         };

//		// Then delete all selected atoms
//		foreach (item, selectedItems())
//		  if (item->type() == Atom::Type)
//		  {
//			atom = dynamic_cast<Atom*>(item);
//			molSet << atom->molecule();
//			m_stack->push(new DelAtom(atom));
//		  }

//		// Cleanup the affected molecules
//		foreach (Molecule* mol, molSet)
//		{
//		  if (mol->canSplit()) m_stack->push(new SplitMol(mol));
//		  if (mol->atoms().isEmpty()) m_stack->push(new DelItem(mol));
//		}

//		// Finally delete all the residues
//		foreach (item, selectedItems()) m_stack->push(new DelItem(item));

//		m_stack->endMacro();
//		keyEvent->accept();
//		break;
//		case Qt::Key_Backspace:
//			m_stack->beginMacro(tr("removing item(s)"));
//			// First delete all selected molecules
//			foreach (item, selectedItems())
//			if (item->type() == Molecule::Type)
//			{
//				m_stack->push(new DelItem(item));
//			}
//			//       // Then delete
//			//       foreach (item, selectedItems())
//			//         if (item->type() == Bond::Type)
//			//         {
//			//           bond = dynamic_cast<Bond*>(item);
//			//           mol = bond->molecule();
//			//           m_stack->push(new DelBond(bond));
//			//           if (mol->canSplit()) m_stack->push(new SplitMol(mol));
//			//         };

//			// Then delete all selected atoms
//			foreach (item, selectedItems())
//			if (item->type() == Atom::Type)
//			{
//				atom = dynamic_cast<Atom*>(item);
//				molSet << atom->molecule();
//				m_stack->push(new DelAtom(atom));
//			}

//			// Cleanup the affected molecules
//			foreach (Molecule* mol, molSet)
//		{
//			if (mol->canSplit()) m_stack->push(new SplitMol(mol));
//			if (mol->atoms().isEmpty()) m_stack->push(new DelItem(mol));
//		}

//			// Finally delete all the residues
//			foreach (item, selectedItems()) m_stack->push(new DelItem(item));

//			m_stack->endMacro();
//			keyEvent->accept();
//			break;


//	  case Qt::Key_Up:
//		m_stack->beginMacro("moving item(s)");
//		foreach (item, selectedItems())
//		  m_stack->push(new MoveItem(item,QPointF(0,-10)));
//		m_stack->endMacro();
//		keyEvent->accept();
//		break;
//	  case Qt::Key_Down:
//		m_stack->beginMacro("moving item(s)");
//		foreach (item, selectedItems())
//		  m_stack->push(new MoveItem(item,QPointF(0,10)));
//		m_stack->endMacro();
//		keyEvent->accept();
//		break;
//	  case Qt::Key_Left:
//		m_stack->beginMacro("moving item(s)");
//		foreach (item, selectedItems())
//		  m_stack->push(new MoveItem(item,QPointF(-10,0)));
//		m_stack->endMacro();
//		keyEvent->accept();
//		break;
//	  case Qt::Key_Right:
//		m_stack->beginMacro("moving item(s)");
//		foreach (item, selectedItems())
//		  m_stack->push(new MoveItem(item,QPointF(10,0)));
//		m_stack->endMacro();
//		keyEvent->accept();
//		break;
//	  case Qt::Key_Escape:
//		clearSelection();
//		break;
//	  default:
//		keyEvent->ignore();
//	}
//	  }

        // execute default behaviour (needed for text tool)
    keyEvent->ignore();
    QGraphicsScene::keyPressEvent(keyEvent);
    update();
    if (keyEvent->isAccepted()) return;
    if (keyEvent->key() == Qt::Key_Escape)
    {
        keyEvent->accept();
        clearSelection();
#if QT_VERSION < 0x050000
        QList<QAction*> directActionChildren, actionChildren = findChildren<QAction*>() ;
        std::copy_if(actionChildren.begin(), actionChildren.end(), std::back_inserter(directActionChildren),
                     [&](QAction* a) { return a->parent() == this; });
        for (QAction* action : directActionChildren)
#else
        foreach(QAction* action, findChildren<QAction*>(QString(), Qt::FindDirectChildrenOnly))
#endif
          if (action->isChecked())
            action->setChecked(false);
    }
  }

  void MolScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
  {
        QMenu contextMenu;
        qDebug() << "context menu";
//        sceneMenu->setParent(&contextMenu);
//        contextMenu.addMenu(sceneMenu);
        qDebug() << "Generated menu";
        foreach(QGraphicsItem* qgItem, selectedItems())
        {
          graphicsItem *item = dynamic_cast<graphicsItem*>(qgItem);
          if (!item) continue;
          item->prepareContextMenu(&contextMenu);
        }

        qDebug() << "-------- context menu for no of items:" << selectedItems().size();
        if (contextMenu.actions().empty())
        {
          d->contextSubMenu()->exec(event->screenPos());
          event->accept();
          return;
        }
        contextMenu.addMenu(d->contextSubMenu()); // TODO memory leak

        contextMenu.exec(event->screenPos());
        event->accept();
  }

  void MolScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
  {
    event->ignore();
    QGraphicsScene::mousePressEvent(event);
    if (event->isAccepted()) return;
    if (event->button() != Qt::LeftButton || event->modifiers()) return;
    d->selectionRectangle->setRect(QRectF(event->scenePos(), event->scenePos()));
    addItem(d->selectionRectangle);
    d->selectionRectangle->show();
    event->accept();
  }

  void MolScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
  {
    event->ignore();
    QGraphicsScene::mouseMoveEvent(event);
    if (event->isAccepted()) return;
    if (!d->selectionRectangle->scene()) return;
    if (!(event->buttons() & Qt::LeftButton)) return;
    d->selectionRectangle->setRect(QRectF(event->buttonDownScenePos(Qt::LeftButton), event->scenePos()));
    QPainterPath selectArea;
    selectArea.addRect(d->selectionRectangle->rect());
    setSelectionArea(selectArea);
    event->accept();
  }

  void MolScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
  {
    event->ignore();
    QGraphicsScene::mouseReleaseEvent(event);
    if(event->isAccepted()) return;
    if (event->button() != Qt::LeftButton) return;
    removeItem(d->selectionRectangle);
    event->accept();
  }

  void MolScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
  {
    if (!event->mimeData()->hasFormat(mimeType())) return;
    if (!event->proposedAction() == Qt::CopyAction) return;
    event->accept();
#if QT_VERSION >= 0x050000
    foreach(QAction* action, findChildren<QAction*>(QString(), Qt::FindDirectChildrenOnly))
      if (action->isCheckable())
        action->setChecked(false);
#else
    foreach(QAction* action, findChildren<QAction*>())
          if (action->isCheckable() && action->parent() == this)
            action->setChecked(false);
#endif
    d->dragItem = new Molecule;
    QXmlStreamReader reader(event->mimeData()->data(mimeType()));
    reader >> *(d->dragItem);
    d->dragItem->setPos(event->pos());
    addItem(d->dragItem);
    updateAll();
  }

  void MolScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
  {
    if (!d->dragItem) return;
    removeItem(d->dragItem);
    delete d->dragItem;
    d->dragItem = 0;
    event->accept();
  }

  void MolScene::dropEvent(QGraphicsSceneDragDropEvent *event)
  {
    if (!d->dragItem) return;
    event->accept();
    stack()->push(new Commands::AddItem(d->dragItem, this, tr("insert molecule")));
  }

  void MolScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
  {
    qDebug() << "drag moving:" << event->proposedAction();
    if (!d->dragItem) return;
    d->dragItem->setPos(event->scenePos());
    event->accept();
  }

  void MolScene::wheelEvent(QGraphicsSceneWheelEvent *event)
  {
    foreach(QGraphicsView* vp, views()) // TODO track back event to originator
    {
      MolView *mvp = qobject_cast<MolView*>(vp);
      if (mvp)
        mvp->scaleView(pow((double)2, -event->delta() / MOUSEWHEELDIVIDER));
    }
  }


  QUndoStack * MolScene::stack()
  {
        return m_stack;
  }

  QList<genericAction *> MolScene::sceneActions() const
  {
        return findChildren<genericAction*>();
  }

} // namespace
