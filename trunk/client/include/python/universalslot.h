
#if !defined(__UNIVERSALSLOT_H__)
#define __UNIVERSALSLOT_H__

#include <Python.h>

#include <QObject>
#include <QMetaMethod>

class cUniversalSlot : public QObject
{
public:
	cUniversalSlot(QObject *sender, const char *signal, PyObject *recipient);
	~cUniversalSlot();

    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    QT_TR_FUNCTIONS
    virtual int qt_metacall(QMetaObject::Call, int, void **);

	// The actual code-glue between qt->python
	void universalslot(void **arguments);

	bool isValid() const;

	PyObject *recipient() const;
protected:
	PyObject *recipient_;
	QMetaMethod slotSignature;
	bool valid;
};

inline PyObject *cUniversalSlot::recipient() const {
	return recipient_;
}

inline bool cUniversalSlot::isValid() const {
	return valid;
}

#endif
