
#include "game/dynamicentity.h"
#include "game/world.h"

cDynamicEntity::cDynamicEntity(unsigned short x, unsigned short y, signed char z, enFacet facet, unsigned int serial) : cEntity(x, y, z, facet) {
	serial_ = serial;
	World->registerDynamic(this);
}

cDynamicEntity::~cDynamicEntity() {
	World->unregisterDynamic(this);
}

void cDynamicEntity::move(unsigned short x, unsigned short y, signed char z) {
	World->removeEntity(this);
	x_ = x;
	y_ = y;
	z_ = z;
	World->addEntity(this);
}
