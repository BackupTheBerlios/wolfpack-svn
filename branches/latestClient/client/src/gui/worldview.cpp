
#include "gui/worldview.h"
#include "gui/label.h"
#include "gui/asciilabel.h"
#include "gui/cursor.h"
#include "gui/gui.h"
#include "game/world.h"
#include "game/mobile.h"
#include "game/dynamicitem.h"
#include "game/targetrequest.h"
#include "network/outgoingpackets.h"
#include "network/uosocket.h"
#include "client.h"
#include "mainwindow.h"
#include <math.h>
#include <QCursor>
#include <QMouseEvent>

const unsigned int sysMessageDecay = 10000;

class cInputField : public cTextField {
public:
	cInputField(int x, int y, int width, int height) : cTextField(x, y, width, height, 3, 0, 0x2486, false, true) {
		// Set background transparency
		background_->setAlpha(0.5);
	}
};

class cSysMessage : public cLabel {
protected:
	unsigned int created_;
public:
	unsigned int created() const;
	void setCreated(unsigned int data);

	cSysMessage(const QString &message, unsigned char font, unsigned short hue) : cLabel(message, font, hue) {
	}
};

inline unsigned int cSysMessage::created() const {
	return created_;
}

inline void cSysMessage::setCreated(unsigned int data) {
	created_ = data;
}

void cWorldView::cleanSysMessages() {
	QList<cControl*> toremove;
	unsigned int currentTime = Utilities::getTicks();

	foreach( cControl* control, controls ) {
		if (control != left && control != right && control != top && control != bottom && control != inputField) {
			cSysMessage *message = (cSysMessage*)(control);
			if (message->created() + sysMessageDecay < currentTime) {
				toremove.append(control);
			}
		}
	}

	foreach( cControl* control, toremove )
	{
		removeControl( control );
		delete control;
	}
}

cWorldView::cWorldView(unsigned short width, unsigned short height) {
	top = new cTiledGumpImage(0xa8c);
	top->setHeight(4);
	top->setAlign(CA_TOP);
	top->setMoveHandle(true);
	addControl(top);

	left = new cTiledGumpImage(0xa8d);
	left->setWidth(5);
	left->setAlign(CA_LEFT);
	left->setMoveHandle(true);
	addControl(left);

	bottom = new cTiledGumpImage(0xa8c);
	bottom->setHeight(4);
	bottom->setAlign(CA_BOTTOM);
	bottom->setMoveHandle(true);
	addControl(bottom);

	right = new cTiledGumpImage(0xa8d);
	right->setWidth(5);
	right->setAlign(CA_RIGHT);
	right->setMoveHandle(true);
	addControl(right);

	setBounds(x_, y_, width, height);

	moving = false;
	nextSysmessageCleanup = Utilities::getTicks() + 250;

	inputField = new cInputField(5, height - 30, width - 10, 25);
	inputField->setVisible(false);
	addControl(inputField);
	connect(inputField, SIGNAL(enterPressed(cTextField*)), this, SLOT(textFieldEnter(cTextField*)));

	currentTarget = 0;
	movementBlocked = false;
	
	wantTabs_ = true; // Want tabs (warmode changes)

	itemTracking = false;
	pickupTimer.setSingleShot(true);
	connect(&pickupTimer, SIGNAL(timeout()), SLOT(pickupItem()));
}

cWorldView::~cWorldView() {
}

void cWorldView::requestTarget(cTargetRequest *target) {
	cancelTarget(); // Cancel current target

	if (target) {
		currentTarget = target;
	}

	Cursor->setCursor(getCursorType());
}

void cWorldView::cancelTarget() {
	if (currentTarget) {
		currentTarget->cancel();
		delete currentTarget;
		currentTarget = 0;
	}

	Cursor->setCursor(getCursorType());
}

cControl *cWorldView::getControl(int x, int y) {
	if (x >= 0 && y >= 0 && x < width_ && y < height_) {
		cControl *ctrl = cContainer::getControl(x, y);
		if (ctrl == inputField) {
			return inputField;
		}
		return this;
	} else {
		return 0;
	}
}

// Stop tracking, otherwise pass to World.
void cWorldView::onMouseUp(QMouseEvent *e) {
	if (moving) {
		moving = false;
		Cursor->setCursor(getCursorType());
	} else if (itemTracking) {
		pickupTimer.stop();
		itemTracking = false;
	} else {
		cWindow::onMouseUp(e);
	}
}

void cWorldView::onClick(QMouseEvent *e) {
	World->onClick(e);
}

// Start tracking if we're touching the border. Otherwise pass to world.
void cWorldView::onMouseDown(QMouseEvent *e) {
	// Try getting the control at that position
	cControl *ctrl = cContainer::getControl(e->x() - x_, e->y() - y_);

	// Start tracking if above border.
	if (ctrl == left || ctrl == right || ctrl == top || ctrl == bottom) {
		tracking = true;
	} else {
		if (e->button() == Qt::RightButton) {
			moving = true;
		} else if (e->button() == Qt::LeftButton) {
			cDynamicItem *item = dynamic_cast<cDynamicItem*>(World->mouseOver());

			if (item && item->canMove() && Utilities::simpleDistance(item->x(), item->y(), World->x(), World->y()) <= 3) {
				itemTracking = true;
				pickupTimer.start(1000);
			}
		}
	}
}

// Move the window around if we're tracking. Otherwise pass to world.
void cWorldView::onMouseMotion(int xrel, int yrel, QMouseEvent *e) {
	if (tracking) {
		cWindow::onMouseMotion(xrel, yrel, e);
	} else if (itemTracking) {
		pickupItem();
	} else {
		// pass to world
		Cursor->setCursor(getCursorType());
	}
}

void cWorldView::addSysMessage(const QString &message, unsigned short hue, unsigned char font) {
	cSysMessage *label = new cSysMessage(message, font, hue);
	label->setAutoSize(false);
	label->setSize(450, 0);
	label->update(); // Create the surface so the height is known
	label->setCreated(Utilities::getTicks());
	label->setPosition(left->width(), bottom->y() - 50 - label->height());
	moveContent(- label->height() - 4);
	addControl(label);
}

void cWorldView::moveContent(int yoffset) {
	QList<cControl*> toremove;

	foreach( cControl* control, controls )
	{
		if (control != left && control != top && control != bottom && control != right && control != inputField){
			if (control->y() + control->height() + yoffset <= top->height()) {
				// The control would become invisible. Queue it for deletion.
				toremove.append(control);
			} else {
				// Move the control up if it would still be visible
				control->setPosition(control->x(), control->y() + yoffset);
			}
		}
	}

	// Remove controls that came out of view
	foreach( cControl* control, toremove )
	{
		removeControl(control);
		delete control;
	}
}

void cWorldView::moveTick() {
	// TEMPORARY HACK UNTIL PLAYER OBJECT + NETWORKING EXISTS
	static unsigned int nextmove = 0;

	movementBlocked = false; // Reset this

	// we're no longer moving
	if (!moving || (Player && Player->isMoving())) {
		return;
	}

	// Don't move if 4 movements have been queued already
	if (UoSocket->sequenceQueueLength() >= 4) {
		return;
	}

	// Check the distance from the center in pixels
	QPoint pos = GLWidget->mapFromGlobal(QCursor::pos());
    int diffx = pos.x() - (x_ + width_ / 2);
	int diffy = pos.y() - (y_ + height_ / 2);
	int distance = (int)sqrt((float)(diffx * diffx + diffy * diffy));
	bool running = distance > width_ / 4;

	// Calculate the duration of the movement
	int duration = 0;
	if (Player) {
		duration = Player->getMoveDuration(running);
	}

	if (nextmove > Utilities::getTicks()) {
		return;
	}

	// Get the direction the cursor is pointing to
	enCursorType cursor = getCursorType();
	int xdiff = 0;
	int ydiff = 0;
	int zdiff = 0;

	switch (cursor) {
		// Up
		case CURSOR_DIR0:
			xdiff = -1;
			ydiff = -1;
			break;
		// Up-Right
		case CURSOR_DIR1:
			xdiff = 0;
			ydiff = -1;
			break;
		// Right
		case CURSOR_DIR2:
			xdiff = 1;
			ydiff = -1;
			break;
		// Down-Right
		case CURSOR_DIR3:
			xdiff = 1;
			ydiff = 0;
			break;
		// Down
		case CURSOR_DIR4:
			xdiff = 1;
			ydiff = 1;
			break;
		// Down-Left
		case CURSOR_DIR5:
			xdiff = 0;
			ydiff = 1;
			break;
		// Left
		case CURSOR_DIR6:
			xdiff = -1;
			ydiff = 1;
			break;
		// Up-Left
		case CURSOR_DIR7:
			xdiff = -1;
			ydiff = 0;
			break;
		// Default
		default:
			movementBlocked = true;
			return; // Do nothing
	};

	// Calculate new direction for the player
	int direction = Utilities::direction(World->x(), World->y(), World->x() + xdiff, World->y() + ydiff);

	// Check if we can even go there
	if (Player && Player->direction() == direction) {
		signed char posz = Player->z();
		if (!World->mayWalk(Player, Player->x() + xdiff, Player->y() + ydiff, posz)) {
			movementBlocked = true;
			return;
		}
		nextmove = Utilities::getTicks() + duration - 50;		
		World->smoothMove(xdiff, ydiff, posz - Player->z(), duration);
	} else if (Player && Player->direction() != direction) {
        Player->setDirection(direction);

		// Send the move request
		UoSocket->send(cMoveRequestPacket(direction, UoSocket->moveSequence()));
		UoSocket->pushSequence(UoSocket->moveSequence());
		if (UoSocket->moveSequence() == 255) {
			UoSocket->setMoveSequence(1);
		} else {
			UoSocket->setMoveSequence(UoSocket->moveSequence() + 1);
		}
		nextmove = Utilities::getTicks() + 75;
	} else {
		World->smoothMove(xdiff, ydiff, zdiff, duration);
	}
}

void cWorldView::targetResponse(cEntity *entity) {
	if (currentTarget) {
		currentTarget->target(entity);
		delete currentTarget;
		currentTarget = 0;
		Cursor->setCursor(getCursorType());
	}
}

enCursorType cWorldView::getCursorType() {
	QPoint pos = GLWidget->mapFromGlobal(QCursor::pos());
	int mx = pos.x();
	int my = pos.y();

	// Return a normal cursor if we're outside the worldview
	if (!moving && (mx < x_ + left->width() || mx >= x_ + width_ - right->width() ||
		my < y_ + top->height() || my >= y_ + height_ - bottom->height())) {
		return CURSOR_NORMAL;
	}

	// Return the targetting cursor if we have a target
	if (currentTarget) {
		return CURSOR_TARGET;
	}

	// Get center of world
	int centerx = x_ + width_ / 2;
	int centery = y_ + height_ / 2;

	// We assume that the cursor and the center of the
	// worldview open a triangle.
	float diffx = (float)(mx - centerx);
	float diffy = (float)(my - centery);

	// Center of the worldview
	if (abs((int)diffx) < 10.0f && abs((int)diffy) < 10.0f) {
		return CURSOR_NORMAL;
	}

    float diagonal = sqrt(diffx * diffx + diffy * diffy); // Calculate diagonal
	float asina = asin(diffy / diagonal); // Calculate the sin of the angle

	const float treshold1 = PI / 8; // Upper treshold
	const float treshold2 = PI / 2 - treshold1; // Lower treshold

	// Since the sin is symmetric, handle both halfs of the screen separately
	if (diffx < 0) {
		if (asina < - treshold2) {
			return CURSOR_DIR0; // Up
		} else if (asina < - treshold1) {
			return CURSOR_DIR7; // Up-Left
		} else if (asina < treshold1) {
			return CURSOR_DIR6; // Left
		} else if (asina < treshold2) {
			return CURSOR_DIR5; // Down-Left
		} else {
			return CURSOR_DIR4; // Down
		}
	} else {
		if (asina < - treshold2) {
			return CURSOR_DIR0; // Up
		} else if (asina < - treshold1) {
			return CURSOR_DIR1; // Up-Left
		} else if (asina < treshold1) {
			return CURSOR_DIR2; // Left
		} else if (asina < treshold2) {
			return CURSOR_DIR3; // Down-Left
		} else {
			return CURSOR_DIR4; // Down
		}
	}

	return CURSOR_DIR0;
}

void cWorldView::onMouseEnter() {
	Cursor->setCursor(getCursorType());
}

void cWorldView::onMouseLeave() {
	// Only rsetore the cursor if we're not tracking the mosue movement
	if (!moving) {
		Cursor->setCursor(CURSOR_NORMAL);
	}
}

void cWorldView::processDoubleClick(QMouseEvent *e) {
	World->onDoubleClick(e);
}

void cWorldView::textFieldEnter(cTextField *ctrl) {
	inputField->setVisible(false);

	QString text = inputField->text();
	inputField->setText("");

	if (!text.isEmpty()) {
		// Send the text to the server...
		cSendUnicodeSpeechPacket packet(SPEECH_REGULAR, text, 0x3b2, 0);		
		UoSocket->send(packet);
	}
}

void cWorldView::getWorldRect(int &x, int &y, int &width, int &height) {
	x = x_ + left->width();
	y = y_ + top->height();
	width = width_ - left->width() - right->width();
	height = height_ - top->height() - bottom->height();
}

void cWorldView::draw(int xoffset, int yoffset) {
	// Remove outdated sysmessages
	if (nextSysmessageCleanup < Utilities::getTicks()) {
		cleanSysMessages();
		nextSysmessageCleanup = Utilities::getTicks() + 250;
	}

	cWindow::draw(xoffset, yoffset);
}

void cWorldView::showInputLine() {
	inputField->setVisible(true);
	Gui->setInputFocus(inputField);
}

void cWorldView::onKeyDown(QKeyEvent *e) {
	if (e->key() == Qt::Key_Tab) {
	} else {
		cWindow::onKeyDown(e);
	}
}

bool cWorldView::acceptsItemDrop(cDynamicItem *item) {
	cEntity *entity = World->mouseOver();

	if (entity && Utilities::simpleDistance(entity->x(), entity->y(), World->x(), World->y()) <= 3) {
		return true;
	} else {
		return false;
	}
}

void cWorldView::dropItem(cDynamicItem *item) {
	cEntity *entity = World->mouseOver();

	if (!entity || Utilities::simpleDistance(entity->x(), entity->y(), World->x(), World->y()) > 3) {
		return;
	}

	// If the entity is a container or stackable item, drop on it
	cDynamicItem *dItem = dynamic_cast<cDynamicItem*>(entity);
	if (dItem && (dItem->tiledata()->isContainer() || dItem->tiledata()->isGeneric())) {
		UoSocket->send(cDropItemPacket(item->serial(), 0xFFFF, 0xFFFF, 0xFF, dItem->serial()));
		Gui->dropItem();
		return;
	}

	// If the entity is a mobile, same procedure
	cMobile *mobile = dynamic_cast<cMobile*>(entity);
	if (mobile) {
		UoSocket->send(cDropItemPacket(item->serial(), 0xFFFF, 0xFFFF, 0xFF, mobile->serial()));
		Gui->dropItem();
		return;
	}

    // Otherwise just drop it on the ground
	UoSocket->send(cDropItemPacket(item->serial(), entity->x(), entity->y(), entity->z(), 0xFFFFFFFF));
	Gui->dropItem();
}

/*
	Pickup the item below the mouse location
*/
void cWorldView::pickupItem() {	
	if (Gui->isDragging() || !World->mouseOver()) {
		return;
	}

	itemTracking = false;
	pickupTimer.stop();

	// Pickup the item below the mousedown position
	cDynamicItem *item = dynamic_cast<cDynamicItem*>(World->mouseOver());

	if (item) {
		if (item && !item->canMove()) {
			return;
		}

		Gui->dragItem(item);
		item->moveToLimbo();
		item->decref();

		// send a drag-request to the server
		UoSocket->send(cGrabItemPacket(item->serial()));
	}
}


cWorldView *WorldView = 0;
