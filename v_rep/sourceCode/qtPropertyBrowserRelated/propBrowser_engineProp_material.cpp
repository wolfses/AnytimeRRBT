#include "propBrowser_engineProp_material.h"

#include "Tt.h"
#include "GV.h"
#include "DynMaterialObject.h"
#include "App.h"

bool CPropBrowserEngineMaterial::_bulletPropertiesExpanded=true;
bool CPropBrowserEngineMaterial::_odePropertiesExpanded=true;
bool CPropBrowserEngineMaterial::_vortexPropertiesExpanded=true;
bool CPropBrowserEngineMaterial::_vortexAutosleepPropertiesExpanded=false;
bool CPropBrowserEngineMaterial::_vortexPrimLinAxisPropertiesExpanded=true;
bool CPropBrowserEngineMaterial::_vortexSecLinAxisPropertiesExpanded=false;
bool CPropBrowserEngineMaterial::_vortexPrimAngAxisPropertiesExpanded=false;
bool CPropBrowserEngineMaterial::_vortexSecAngAxisPropertiesExpanded=false;
bool CPropBrowserEngineMaterial::_vortexNormAngAxisPropertiesExpanded=false;
bool CPropBrowserEngineMaterial::_vortexPrimLinAxisVectorPropertiesExpanded=false;
int CPropBrowserEngineMaterial::_dlgSizeX=500;
int CPropBrowserEngineMaterial::_dlgSizeY=750;
int CPropBrowserEngineMaterial::_dlgPosX=800;
int CPropBrowserEngineMaterial::_dlgPosY=100;
int CPropBrowserEngineMaterial::_splitterPos=250;

CPropBrowserEngineMaterial::CPropBrowserEngineMaterial(QWidget* pParent) : QtTreePropertyBrowser(pParent)
{
	show(NULL);
}

CPropBrowserEngineMaterial::~CPropBrowserEngineMaterial()
{
	enableNotifications(false);

	_bulletPropertiesExpanded=isExpanded(bulletGroupItem);
	_odePropertiesExpanded=isExpanded(odeGroupItem);
	_vortexPropertiesExpanded=isExpanded(vortexGroupItem);
	_vortexAutosleepPropertiesExpanded=isExpanded(autoSleepItem);
	_vortexPrimLinAxisPropertiesExpanded=isExpanded(linAxisPrimItem);
	_vortexSecLinAxisPropertiesExpanded=isExpanded(linAxisSecItem);
	_vortexPrimAngAxisPropertiesExpanded=isExpanded(angAxisPrimItem);
	_vortexSecAngAxisPropertiesExpanded=isExpanded(angAxisSecItem);
	_vortexNormAngAxisPropertiesExpanded=isExpanded(angAxisNormItem);
	_vortexPrimLinAxisVectorPropertiesExpanded=isExpanded(linAxisPrimVectorItem);

	if ((windowState()&Qt::WindowMaximized)==0)
	{
		QRect geom(geometry());
		_dlgSizeX=geom.width();
		_dlgSizeY=geom.height();
		_dlgPosX=geom.x();
		_dlgPosY=geom.y();
		_splitterPos=splitterPosition();
	}

	// following 2 needed, otherwise the prop browser will eventually crash "out of the blue" (it keeps track of things in a static map)
	unsetFactoryForManager(buttonManager); // this will output a Qt error/warning to the console. Not good.
	unsetFactoryForManager(variantManager);

	delete buttonFactory;
	delete buttonManager;
	delete variantFactory;
	delete variantManager;
}


void CPropBrowserEngineMaterial::reject()
{
	done(QDialog::Accepted);
}

void CPropBrowserEngineMaterial::accept()
{
	done(QDialog::Accepted);
}

void CPropBrowserEngineMaterial::show(QWidget* parentWindow)
{
	setPropertiesWithoutValueMarked(true); // so that group properties stand out
	setRootIsDecorated(true); // keep true, otherwise subcategories won't work correctly
	setAlternatingRowColors(true);
	setHeaderVisible(true);
	//setIndentation(60);
	//setStatusTip("bla");
	//setToolTip("Bla");
	setResizeMode(QtTreePropertyBrowser::Interactive);
	setSplitterPosition(_splitterPos);
	setWindowTitle("Physics Engines Properties - Material");


	variantManager = new QtVariantPropertyManager();
	variantFactory = new QtVariantEditorFactory();
	setFactoryForManager(variantManager,variantFactory);


	buttonManager = new ButtonEditManager();
	buttonFactory  = new PushButtonEditFactory();
	setFactoryForManager(buttonManager,buttonFactory);

	p_materialName = variantManager->addProperty(QVariant::String,"Material name");
	QtBrowserItem* anItem=addProperty(p_materialName);
	setBackgroundColor(anItem,QTPROPERTYBROWSER_COLOR_GREY);

	p_duplicateButton = buttonManager->addProperty("");
	buttonManager->setValue(p_duplicateButton,"Duplicate");
	anItem=addProperty(p_duplicateButton);
	setBackgroundColor(anItem,QTPROPERTYBROWSER_COLOR_GREY);


	QtProperty *bulletGroup = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"Bullet properties");
	bulletGroup->theBrightness=140;
	bulletGroupItem=addProperty(bulletGroup);
	setBackgroundColor(bulletGroupItem,QTPROPERTYBROWSER_COLOR_RED);
	setExpanded(bulletGroupItem,_bulletPropertiesExpanded);

	QtProperty *odeGroup = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"ODE properties");
	odeGroup->theBrightness=140;
	odeGroupItem=addProperty(odeGroup);
	setBackgroundColor(odeGroupItem,QTPROPERTYBROWSER_COLOR_GREEN);
	setExpanded(odeGroupItem,_odePropertiesExpanded);

	QtProperty *vortexGroup = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"Vortex properties");
	vortexGroup->theBrightness=140;
	vortexGroupItem=addProperty(vortexGroup);
	setBackgroundColor(vortexGroupItem,QTPROPERTYBROWSER_COLOR_BLUE);
	setExpanded(vortexGroupItem,_vortexPropertiesExpanded);


	p_bulletFriction = variantManager->addProperty(QVariant::String,"");
	bulletGroup->addSubProperty(p_bulletFriction);
	p_bulletRestitution = variantManager->addProperty(QVariant::String,"");
	bulletGroup->addSubProperty(p_bulletRestitution);
	p_bulletLinDamping = variantManager->addProperty(QVariant::String,"");
	bulletGroup->addSubProperty(p_bulletLinDamping);
	p_bulletAngDamping = variantManager->addProperty(QVariant::String,"");
	bulletGroup->addSubProperty(p_bulletAngDamping);
	p_bulletStickyContact = variantManager->addProperty(QVariant::Bool,"");
	bulletGroup->addSubProperty(p_bulletStickyContact);
	p_bulletAutoShrink = variantManager->addProperty(QVariant::Bool,"");
	bulletGroup->addSubProperty(p_bulletAutoShrink);
	p_bulletCustomMarginScaling = variantManager->addProperty(QVariant::Bool,"");
	bulletGroup->addSubProperty(p_bulletCustomMarginScaling);
	p_bulletMarginScalingFactor = variantManager->addProperty(QVariant::String,"");
	bulletGroup->addSubProperty(p_bulletMarginScalingFactor);

	p_odeFriction = variantManager->addProperty(QVariant::String,"");
	odeGroup->addSubProperty(p_odeFriction);
	p_odeMaxContacts = variantManager->addProperty(QVariant::String,"");
	odeGroup->addSubProperty(p_odeMaxContacts);
	p_odeSoftERP = variantManager->addProperty(QVariant::String,"");
	odeGroup->addSubProperty(p_odeSoftERP);
	p_odeSoftCFM = variantManager->addProperty(QVariant::String,"");
	odeGroup->addSubProperty(p_odeSoftCFM);
	p_odeLinDamping = variantManager->addProperty(QVariant::String,"");
	odeGroup->addSubProperty(p_odeLinDamping);
	p_odeAngDamping = variantManager->addProperty(QVariant::String,"");
	odeGroup->addSubProperty(p_odeAngDamping);

	p_vortexRestitution = variantManager->addProperty(QVariant::String,"");
	vortexGroup->addSubProperty(p_vortexRestitution);
	p_vortexRestitutionThreshold = variantManager->addProperty(QVariant::String,"");
	vortexGroup->addSubProperty(p_vortexRestitutionThreshold);
	p_vortexCompliance = variantManager->addProperty(QVariant::String,"");
	vortexGroup->addSubProperty(p_vortexCompliance);
	p_vortexDamping = variantManager->addProperty(QVariant::String,"");
	vortexGroup->addSubProperty(p_vortexDamping);
	p_vortexAdhesiveForce = variantManager->addProperty(QVariant::String,"");
	vortexGroup->addSubProperty(p_vortexAdhesiveForce);
	p_vortexLinVelocityDamping = variantManager->addProperty(QVariant::String,"");
	vortexGroup->addSubProperty(p_vortexLinVelocityDamping);
	p_vortexAngVelocityDamping = variantManager->addProperty(QVariant::String,"");
	vortexGroup->addSubProperty(p_vortexAngVelocityDamping);
	p_vortexAutoAngularDampingEnabled = variantManager->addProperty(QVariant::Bool,"");
	vortexGroup->addSubProperty(p_vortexAutoAngularDampingEnabled);
	p_vortexAutoAngularDampingTensionRatio = variantManager->addProperty(QVariant::String,"");
	vortexGroup->addSubProperty(p_vortexAutoAngularDampingTensionRatio);
	p_vortexSkinThickness = variantManager->addProperty(QVariant::String,"");
	vortexGroup->addSubProperty(p_vortexSkinThickness);
	p_vortexAutoSlip = variantManager->addProperty(QVariant::Bool,"");
	vortexGroup->addSubProperty(p_vortexAutoSlip);
	p_vortexFastMoving = variantManager->addProperty(QVariant::Bool,"");
	vortexGroup->addSubProperty(p_vortexFastMoving);
	p_vortexPureAsConvex = variantManager->addProperty(QVariant::Bool,"");
	vortexGroup->addSubProperty(p_vortexPureAsConvex);
	p_vortexConvexAsRandom = variantManager->addProperty(QVariant::Bool,"");
	vortexGroup->addSubProperty(p_vortexConvexAsRandom);
	p_vortexRandomAsTerrain = variantManager->addProperty(QVariant::Bool,"");
	vortexGroup->addSubProperty(p_vortexRandomAsTerrain);

	QtProperty* autoSleep = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"Auto-sleep");
	vortexGroup->addSubProperty(autoSleep);
	autoSleepItem=getSubPropertyBrowserItem(vortexGroupItem,autoSleep);
	setExpanded(autoSleepItem,_vortexAutosleepPropertiesExpanded);


	p_autoSleepLinSpeedThreshold = variantManager->addProperty(QVariant::String,"");
	autoSleep->addSubProperty(p_autoSleepLinSpeedThreshold);
	p_autoSleepLinAccelThreshold = variantManager->addProperty(QVariant::String,"");
	autoSleep->addSubProperty(p_autoSleepLinAccelThreshold);
	p_autoSleepAngSpeedThreshold = variantManager->addProperty(QVariant::String,"");
	autoSleep->addSubProperty(p_autoSleepAngSpeedThreshold);
	p_autoSleepAngAccelThreshold = variantManager->addProperty(QVariant::String,"");
	autoSleep->addSubProperty(p_autoSleepAngAccelThreshold);
	p_autoSleepStepsThreshold = variantManager->addProperty(QVariant::String,"");
	autoSleep->addSubProperty(p_autoSleepStepsThreshold);


	linAxisPrim = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"");
	vortexGroup->addSubProperty(linAxisPrim);
	linAxisPrimItem=getSubPropertyBrowserItem(vortexGroupItem,linAxisPrim);
	setExpanded(linAxisPrimItem,_vortexPrimLinAxisPropertiesExpanded);

	linAxisSec = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"");
	vortexGroup->addSubProperty(linAxisSec);
	linAxisSecItem=getSubPropertyBrowserItem(vortexGroupItem,linAxisSec);
	setExpanded(linAxisSecItem,_vortexSecLinAxisPropertiesExpanded);

	angAxisPrim = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"");
	vortexGroup->addSubProperty(angAxisPrim);
	angAxisPrimItem=getSubPropertyBrowserItem(vortexGroupItem,angAxisPrim);
	setExpanded(angAxisPrimItem,_vortexPrimAngAxisPropertiesExpanded);

	angAxisSec = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"");
	vortexGroup->addSubProperty(angAxisSec);
	angAxisSecItem=getSubPropertyBrowserItem(vortexGroupItem,angAxisSec);
	setExpanded(angAxisSecItem,_vortexSecAngAxisPropertiesExpanded);

	angAxisNorm = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"");
	vortexGroup->addSubProperty(angAxisNorm);
	angAxisNormItem=getSubPropertyBrowserItem(vortexGroupItem,angAxisNorm);
	setExpanded(angAxisNormItem,_vortexNormAngAxisPropertiesExpanded);

	linAxisPrimVector = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"");
	linAxisPrim->addSubProperty(linAxisPrimVector);
	linAxisPrimVectorItem=getSubPropertyBrowserItem(linAxisPrimItem,linAxisPrimVector);
	setExpanded(linAxisPrimVectorItem,_vortexPrimLinAxisVectorPropertiesExpanded);

	p_primAxisVectorX = variantManager->addProperty(QVariant::String,"");
	linAxisPrimVector->addSubProperty(p_primAxisVectorX);
	p_primAxisVectorY = variantManager->addProperty(QVariant::String,"");
	linAxisPrimVector->addSubProperty(p_primAxisVectorY);
	p_primAxisVectorZ = variantManager->addProperty(QVariant::String,"");
	linAxisPrimVector->addSubProperty(p_primAxisVectorZ);

	QStringList frictionModelEnum;
	frictionModelEnum << "Box" << "Scaled box" << "Box proportional low" << "Box proportional high" << "Scaled box fast" << "Neutral" << "None";
	p_vortexFrictionModelLinPrim = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),"");
	p_vortexFrictionModelLinPrim->setAttribute("enumNames", frictionModelEnum);
	linAxisPrim->addSubProperty(p_vortexFrictionModelLinPrim);
	p_vortexFrictionCoeffLinPrim = variantManager->addProperty(QVariant::String,"");
	linAxisPrim->addSubProperty(p_vortexFrictionCoeffLinPrim);
	p_vortexStaticFrictionScaleLinPrim = variantManager->addProperty(QVariant::String,"");
	linAxisPrim->addSubProperty(p_vortexStaticFrictionScaleLinPrim);
	p_vortexSlipLinPrim = variantManager->addProperty(QVariant::String,"");
	linAxisPrim->addSubProperty(p_vortexSlipLinPrim);
	p_vortexSlideLinPrim = variantManager->addProperty(QVariant::String,"");
	linAxisPrim->addSubProperty(p_vortexSlideLinPrim);

	p_vortexLinSecFollowsLinPrim = variantManager->addProperty(QVariant::Bool,"");
	linAxisSec->addSubProperty(p_vortexLinSecFollowsLinPrim);
	p_vortexFrictionModelLinSec = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),"");
	p_vortexFrictionModelLinSec->setAttribute("enumNames", frictionModelEnum);
	linAxisSec->addSubProperty(p_vortexFrictionModelLinSec);
	p_vortexFrictionCoeffLinSec = variantManager->addProperty(QVariant::String,"");
	linAxisSec->addSubProperty(p_vortexFrictionCoeffLinSec);
	p_vortexStaticFrictionScaleLinSec = variantManager->addProperty(QVariant::String,"");
	linAxisSec->addSubProperty(p_vortexStaticFrictionScaleLinSec);
	p_vortexSlipLinSec = variantManager->addProperty(QVariant::String,"");
	linAxisSec->addSubProperty(p_vortexSlipLinSec);
	p_vortexSlideLinSec = variantManager->addProperty(QVariant::String,"");
	linAxisSec->addSubProperty(p_vortexSlideLinSec);

	p_vortexFrictionModelAngPrim = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),"");
	p_vortexFrictionModelAngPrim->setAttribute("enumNames", frictionModelEnum);
	angAxisPrim->addSubProperty(p_vortexFrictionModelAngPrim);
	p_vortexFrictionCoeffAngPrim = variantManager->addProperty(QVariant::String,"");
	angAxisPrim->addSubProperty(p_vortexFrictionCoeffAngPrim);
	p_vortexStaticFrictionScaleAngPrim = variantManager->addProperty(QVariant::String,"");
	angAxisPrim->addSubProperty(p_vortexStaticFrictionScaleAngPrim);
	p_vortexSlipAngPrim = variantManager->addProperty(QVariant::String,"");
	angAxisPrim->addSubProperty(p_vortexSlipAngPrim);
	p_vortexSlideAngPrim = variantManager->addProperty(QVariant::String,"");
	angAxisPrim->addSubProperty(p_vortexSlideAngPrim);

	p_vortexAngSecFollowsAngPrim = variantManager->addProperty(QVariant::Bool,"");
	angAxisSec->addSubProperty(p_vortexAngSecFollowsAngPrim);
	p_vortexFrictionModelAngSec = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),"");
	p_vortexFrictionModelAngSec->setAttribute("enumNames", frictionModelEnum);
	angAxisSec->addSubProperty(p_vortexFrictionModelAngSec);
	p_vortexFrictionCoeffAngSec = variantManager->addProperty(QVariant::String,"");
	angAxisSec->addSubProperty(p_vortexFrictionCoeffAngSec);
	p_vortexStaticFrictionScaleAngSec = variantManager->addProperty(QVariant::String,"");
	angAxisSec->addSubProperty(p_vortexStaticFrictionScaleAngSec);
	p_vortexSlipAngSec = variantManager->addProperty(QVariant::String,"");
	angAxisSec->addSubProperty(p_vortexSlipAngSec);
	p_vortexSlideAngSec = variantManager->addProperty(QVariant::String,"");
	angAxisSec->addSubProperty(p_vortexSlideAngSec);

	p_vortexAngNormFollowsAngPrim = variantManager->addProperty(QVariant::Bool,"");
	angAxisNorm->addSubProperty(p_vortexAngNormFollowsAngPrim);
	p_vortexFrictionModelAngNorm = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),"");
	p_vortexFrictionModelAngNorm->setAttribute("enumNames", frictionModelEnum);
	angAxisNorm->addSubProperty(p_vortexFrictionModelAngNorm);
	p_vortexFrictionCoeffAngNorm = variantManager->addProperty(QVariant::String,"");
	angAxisNorm->addSubProperty(p_vortexFrictionCoeffAngNorm);
	p_vortexStaticFrictionScaleAngNorm = variantManager->addProperty(QVariant::String,"");
	angAxisNorm->addSubProperty(p_vortexStaticFrictionScaleAngNorm);
	p_vortexSlipAngNorm = variantManager->addProperty(QVariant::String,"");
	angAxisNorm->addSubProperty(p_vortexSlipAngNorm);
	p_vortexSlideAngNorm = variantManager->addProperty(QVariant::String,"");
	angAxisNorm->addSubProperty(p_vortexSlideAngNorm);

	refresh();

	enableNotifications(true);

//	setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint);
//	setWindowModality(Qt::ApplicationModal);
	setGeometry(_dlgPosX,_dlgPosY,_dlgSizeX,_dlgSizeY);
}

void CPropBrowserEngineMaterial::enableNotifications(bool enable)
{
	if (enable)
	{
		QObject::connect(variantManager, SIGNAL(valueChanged(QtProperty*,QVariant)),this,SLOT(catchPropertyChanges(QtProperty*,QVariant)),Qt::QueuedConnection);
		QObject::connect(buttonManager,SIGNAL(valueChanged(QtProperty*,QString)),this,SLOT(catchPropertyChangesString(QtProperty*,QString)),Qt::QueuedConnection);
	}
	else
	{
		QObject::disconnect(variantManager, SIGNAL(valueChanged(QtProperty*,QVariant)),this,SLOT(catchPropertyChanges(QtProperty*,QVariant)));
		QObject::disconnect(buttonManager,SIGNAL(valueChanged(QtProperty*,QString)),this,SLOT(catchPropertyChangesString(QtProperty*,QString)));
	}
}

void CPropBrowserEngineMaterial::refresh()
{
	CShape* it=(CShape*)App::ct->objCont->getLastSelection();
	CGeomWrap* geom=it->geomData->geomInfo;
	CDynMaterialObject* mat=geom->getDynMaterialObject();
	bool editable=mat->getEditableTag();

	// Material name:
	p_materialName->setValue(mat->getObjectName().c_str());
	p_materialName->setEnabled(editable);

	// Duplicate button:
	if (editable)
		p_duplicateButton->setPropertyName("Duplicate material");
	else
		p_duplicateButton->setPropertyName("Duplicate and edit material");

	// Bullet property names:
	p_bulletFriction->setPropertyName("Friction");
	p_bulletRestitution->setPropertyName("Restitution");
	p_bulletLinDamping->setPropertyName("Linear damping");
	p_bulletAngDamping->setPropertyName("Angular damping");
	p_bulletStickyContact->setPropertyName("Sticky contact");
	p_bulletAutoShrink->setPropertyName("Auto-shrink convex mesh");
	p_bulletCustomMarginScaling->setPropertyName("Custom collision margin");
	p_bulletMarginScalingFactor->setPropertyName("Custom collision margin factor");

	// Bullet parameters:
	p_bulletFriction->setValue(tt::floatToString(mat->getBulletFriction(),false).c_str());
	p_bulletFriction->setEnabled(editable);
	p_bulletRestitution->setValue(tt::floatToString(mat->getBulletRestitution(),false).c_str());
	p_bulletRestitution->setEnabled(editable);
	p_bulletLinDamping->setValue(tt::floatToString(mat->getBulletLinearDamping(),false).c_str());
	p_bulletLinDamping->setEnabled(editable);
	p_bulletAngDamping->setValue(tt::floatToString(mat->getBulletAngularDamping(),false).c_str());
	p_bulletAngDamping->setEnabled(editable);
	p_bulletStickyContact->setValue(mat->getBulletStickyContact());
	p_bulletStickyContact->setEnabled(editable);
	p_bulletAutoShrink->setValue(mat->getBulletAutoShrinkConvexMesh());
	p_bulletAutoShrink->setEnabled(editable);
	bool convexAndNotPure=(geom->isConvex()&&(!geom->isPure()));
	p_bulletCustomMarginScaling->setValue(mat->getBulletNonDefaultCollisionMargin(convexAndNotPure));
	p_bulletCustomMarginScaling->setEnabled(editable);
	p_bulletMarginScalingFactor->setValue(tt::floatToString(mat->getBulletNonDefaultCollisionMarginFactor(convexAndNotPure),false).c_str());
	p_bulletMarginScalingFactor->setEnabled(mat->getBulletNonDefaultCollisionMargin(convexAndNotPure));


	// ODE property names:
	p_odeFriction->setPropertyName("Friction");
	p_odeMaxContacts->setPropertyName("Maximum contacts");
	p_odeSoftERP->setPropertyName("Soft ERP");
	p_odeSoftCFM->setPropertyName("Soft CFM");
	p_odeLinDamping->setPropertyName("Linear damping");
	p_odeAngDamping->setPropertyName("Angular damping");

	// ODE parameters:
	p_odeFriction->setValue(tt::floatToString(mat->getOdeFriction(),false).c_str());
	p_odeFriction->setEnabled(editable);
	p_odeMaxContacts->setValue(tt::intToString(mat->getOdeMaxContacts()).c_str());
	p_odeMaxContacts->setEnabled(editable);
	p_odeSoftERP->setValue(tt::floatToString(mat->getOdeSoftERP(),false).c_str());
	p_odeSoftERP->setEnabled(editable);
	p_odeSoftCFM->setValue(tt::floatToString(mat->getOdeSoftCFM(),false).c_str());
	p_odeSoftCFM->setEnabled(editable);
	p_odeLinDamping->setValue(tt::floatToString(mat->getOdeLinearDamping(),false).c_str());
	p_odeLinDamping->setEnabled(editable);
	p_odeAngDamping->setValue(tt::floatToString(mat->getOdeAngularDamping(),false).c_str());
	p_odeAngDamping->setEnabled(editable);


	// Vortex property names:
	p_vortexRestitution->setPropertyName("Restitution");
	p_vortexRestitutionThreshold->setPropertyName("Restitution threshold");
	p_vortexCompliance->setPropertyName(gv::formatUnitStr("Compliance","s^2/k").c_str());
	p_vortexDamping->setPropertyName(gv::formatUnitStr("Damping","k/s").c_str());
	p_vortexAdhesiveForce->setPropertyName(gv::formatUnitStr("Adhesive force","k*m/s^2").c_str());
	p_vortexLinVelocityDamping->setPropertyName(gv::formatUnitStr("Linear velocity damping","k/s").c_str());
	p_vortexAngVelocityDamping->setPropertyName(gv::formatUnitStr("Angular velocity damping","k*m^2/s").c_str());
	p_vortexAutoAngularDampingEnabled->setPropertyName("Auto angular damping enabled");
	p_vortexAutoAngularDampingTensionRatio->setPropertyName("Auto angular damping tension ratio");
	p_vortexSkinThickness->setPropertyName(gv::formatUnitStr("Skin thickness","m").c_str());
	p_vortexAutoSlip->setPropertyName("Auto-slip enabled");
	p_vortexFastMoving->setPropertyName("Fast moving");
	p_vortexPureAsConvex->setPropertyName("Treat pure shape as VxConvexMesh");
	p_vortexConvexAsRandom->setPropertyName("Treat convex shape as VxTriangleMeshBVTree");
	p_vortexRandomAsTerrain->setPropertyName("Treat random shape as VxTriangleMeshUVGrid");
	p_autoSleepLinSpeedThreshold->setPropertyName(gv::formatUnitStr("Threshold linear speed","m/s").c_str());
	p_autoSleepLinAccelThreshold->setPropertyName(gv::formatUnitStr("Threshold linear acceleration","m/s^2").c_str());
	p_autoSleepAngSpeedThreshold->setPropertyName(gv::formatUnitStr("Threshold angular speed","r/s").c_str());
	p_autoSleepAngAccelThreshold->setPropertyName(gv::formatUnitStr("Threshold angular acceleration","r/s^2").c_str());
	p_autoSleepStepsThreshold->setPropertyName("Threshold steps");

	p_vortexFrictionModelLinPrim->setPropertyName("Friction model");
	p_vortexFrictionCoeffLinPrim->setPropertyName("Friction coefficient");
	p_vortexStaticFrictionScaleLinPrim->setPropertyName("Static friction scale");
	p_vortexSlipLinPrim->setPropertyName(gv::formatUnitStr("Slip","s/k").c_str());
	p_vortexSlideLinPrim->setPropertyName(gv::formatUnitStr("Slide","m/s").c_str());

	p_primAxisVectorX->setPropertyName("X");
	p_primAxisVectorY->setPropertyName("Y");
	p_primAxisVectorZ->setPropertyName("Z");

	p_vortexLinSecFollowsLinPrim->setPropertyName("Use same values as linear primary axis");
	p_vortexFrictionModelLinSec->setPropertyName("Friction model");
	p_vortexFrictionCoeffLinSec->setPropertyName("Friction coefficient");
	p_vortexStaticFrictionScaleLinSec->setPropertyName("Static friction scale");
	p_vortexSlipLinSec->setPropertyName(gv::formatUnitStr("Slip","s/k").c_str());
	p_vortexSlideLinSec->setPropertyName(gv::formatUnitStr("Slide","m/s").c_str());

	p_vortexFrictionModelAngPrim->setPropertyName("Friction model");
	p_vortexFrictionCoeffAngPrim->setPropertyName("Friction coefficient");
	p_vortexStaticFrictionScaleAngPrim->setPropertyName("Static friction scale");
	p_vortexSlipAngPrim->setPropertyName(gv::formatUnitStr("Slip","s/(k*m^2)").c_str());
	p_vortexSlideAngPrim->setPropertyName(gv::formatUnitStr("Slide","r/s").c_str());

	p_vortexAngSecFollowsAngPrim->setPropertyName("Use same values as angular primary axis");
	p_vortexFrictionModelAngSec->setPropertyName("Friction model");
	p_vortexFrictionCoeffAngSec->setPropertyName("Friction coefficient");
	p_vortexStaticFrictionScaleAngSec->setPropertyName("Static friction scale");
	p_vortexSlipAngSec->setPropertyName(gv::formatUnitStr("Slip","s/(k*m^2)").c_str());
	p_vortexSlideAngSec->setPropertyName(gv::formatUnitStr("Slide","r/s").c_str());

	p_vortexAngNormFollowsAngPrim->setPropertyName("Use same values as angular primary axis");
	p_vortexFrictionModelAngNorm->setPropertyName("Friction model");
	p_vortexFrictionCoeffAngNorm->setPropertyName("Friction coefficient");
	p_vortexStaticFrictionScaleAngNorm->setPropertyName("Static friction scale");
	p_vortexSlipAngNorm->setPropertyName(gv::formatUnitStr("Slip","s/(k*m^2)").c_str());
	p_vortexSlideAngNorm->setPropertyName(gv::formatUnitStr("Slide","r/s").c_str());


	// Vortex parameters:
	std::vector<float> floatParams;
	std::vector<int> intParams;
	mat->getVortexFloatParams(floatParams);
	mat->getVortexIntParams(intParams);
	const float frictionCoeff_primary_linearAxis=floatParams[0];
	const float frictionCoeff_secondary_linearAxis=floatParams[1];
	const float frictionCoeff_primary_angularAxis=floatParams[2];
	const float frictionCoeff_secondary_angularAxis=floatParams[3];
	const float frictionCoeff_normal_angularAxis=floatParams[4];
	const float staticFrictionScale_primary_linearAxis=floatParams[5];
	const float staticFrictionScale_secondary_linearAxis=floatParams[6];
	const float staticFrictionScale_primary_angularAxis=floatParams[7];
	const float staticFrictionScale_secondary_angularAxis=floatParams[8];
	const float staticFrictionScale_normal_angularAxis=floatParams[9];
	const float compliance=tt::floatToUserFloat(floatParams[10],gv::secondsToUser*gv::secondsToUser/gv::kilogramsToUser,true);
	const float damping=tt::floatToUserFloat(floatParams[11],gv::kilogramsToUser/gv::secondsToUser,true);
	const float restitution=floatParams[12];
	const float restitutionThreshold=floatParams[13];
	const float adhesiveForce=tt::floatToUserFloat(floatParams[14],gv::forceToUser,true);
	const float linearVelocityDamping=tt::floatToUserFloat(floatParams[15],gv::kilogramsToUser/gv::secondsToUser,true);
	const float angularVelocityDamping=tt::floatToUserFloat(floatParams[16],gv::kilogramsToUser*gv::meterToUser*gv::meterToUser/gv::secondsToUser,true);
	const float slide_primary_linearAxis=tt::floatToUserFloat(floatParams[17],gv::linearVelToUser,true);
	const float slide_secondary_linearAxis=tt::floatToUserFloat(floatParams[18],gv::linearVelToUser,true);
	const float slide_primary_angularAxis=tt::floatToUserFloat(floatParams[19],gv::angularVelToUser,true);
	const float slide_secondary_angularAxis=tt::floatToUserFloat(floatParams[20],gv::angularVelToUser,true);
	const float slide_normal_angularAxis=tt::floatToUserFloat(floatParams[21],gv::angularVelToUser,true);
	const float slip_primary_linearAxis=tt::floatToUserFloat(floatParams[22],gv::secondsToUser/gv::kilogramsToUser,true);
	const float slip_secondary_linearAxis=tt::floatToUserFloat(floatParams[23],gv::secondsToUser/gv::kilogramsToUser,true);
	const float slip_primary_angularAxis=tt::floatToUserFloat(floatParams[24],gv::secondsToUser/(gv::kilogramsToUser*gv::meterToUser*gv::meterToUser),true);
	const float slip_secondary_angularAxis=tt::floatToUserFloat(floatParams[25],gv::secondsToUser/(gv::kilogramsToUser*gv::meterToUser*gv::meterToUser),true);
	const float slip_normal_angularAxis=tt::floatToUserFloat(floatParams[26],gv::secondsToUser/(gv::kilogramsToUser*gv::meterToUser*gv::meterToUser),true);
	const float autoSleep_linear_speed_threshold=tt::floatToUserFloat(floatParams[27],gv::linearVelToUser,true);
	const float autoSleep_linear_accel_threshold=tt::floatToUserFloat(floatParams[28],gv::linearAccelToUser,true);
	const float autoSleep_angular_speed_threshold=tt::floatToUserFloat(floatParams[29],gv::angularVelToUser,true);
	const float autoSleep_angular_accel_threshold=tt::floatToUserFloat(floatParams[30],gv::angularAccelToUser,true);
	const float skin_thickness=tt::floatToUserFloat(floatParams[31],gv::meterToUser,true);;
	const float autoAngularDampingTensionRatio=tt::floatToUserFloat(floatParams[32],1,true);
	const float primaryAxisVectorX=tt::floatToUserFloat(floatParams[33],1,false);
	const float primaryAxisVectorY=tt::floatToUserFloat(floatParams[34],1,false);
	const float primaryAxisVectorZ=tt::floatToUserFloat(floatParams[35],1,false);


	const int frictionModel_primary_linearAxis=intParams[0];
	const int frictionModel_secondary_linearAxis=intParams[1];
	const int frictionModel_primary_angularAxis=intParams[2];
	const int frictionModel_secondary_angularAxis=intParams[3];
	const int frictionModel_normal_angularAxis=intParams[4];
	const bool treatPureShapesAsConvexShapes=((intParams[5]&1)!=0);
	const bool treatConvexShapesAsRandomShapes=((intParams[5]&2)!=0);
	const bool treatRandomShapesAsTerrain=((intParams[5]&4)!=0);
	const bool fastMoving=((intParams[5]&8)!=0);
	const bool autoSlip=((intParams[5]&16)!=0);
	const bool linearSecFollowsLinearPrim=((intParams[5]&32)!=0);
	const bool angularSecFollowsAngularPrim=((intParams[5]&64)!=0);
	const bool angularNormFollowsAngularPrim=((intParams[5]&128)!=0);
	const bool autoAngularDampingEnabled=((intParams[5]&256)!=0);
	const int autoSleepStepLiveThreshold=intParams[6];
	// const int uniqueId=intParams[7];

	std::string linAxis1Text("Linear primary axis (friction: ");
	std::string linAxis2Text("Linear secondary axis (friction: ");
	std::string angAxis1Text("Angular primary axis (friction: ");
	std::string angAxis2Text("Angular secondary axis (friction: ");
	std::string angAxis3Text("Angular normal axis (friction: ");

	std::string linAxisVectorText("Axis orientation: [");



	p_vortexRestitution->setValue(tt::floatToString(restitution,false).c_str());
	p_vortexRestitution->setEnabled(editable);
	p_vortexRestitutionThreshold->setValue(tt::floatToString(restitutionThreshold,false).c_str());
	p_vortexRestitutionThreshold->setEnabled(editable);
	p_vortexCompliance->setValue(tt::floatToString(compliance,false).c_str());
	p_vortexCompliance->setEnabled(editable);
	p_vortexDamping->setValue(tt::floatToString(damping,false).c_str());
	p_vortexDamping->setEnabled(editable);
	p_vortexAdhesiveForce->setValue(tt::floatToString(adhesiveForce,false).c_str());
	p_vortexAdhesiveForce->setEnabled(editable);
	p_vortexLinVelocityDamping->setValue(tt::floatToString(linearVelocityDamping,false).c_str());
	p_vortexLinVelocityDamping->setEnabled(editable);
	p_vortexAngVelocityDamping->setValue(tt::floatToString(angularVelocityDamping,false).c_str());
	p_vortexAngVelocityDamping->setEnabled(editable);
	p_vortexAutoAngularDampingEnabled->setValue(autoAngularDampingEnabled);
	p_vortexAutoAngularDampingEnabled->setEnabled(editable);
	p_vortexAutoAngularDampingTensionRatio->setValue(tt::floatToString(autoAngularDampingTensionRatio,false).c_str());
	p_vortexAutoAngularDampingTensionRatio->setEnabled(editable&&autoAngularDampingEnabled);
	p_vortexSkinThickness->setValue(tt::floatToString(skin_thickness,false).c_str());
	p_vortexSkinThickness->setEnabled(editable);
	p_vortexAutoSlip->setValue(autoSlip);
	p_vortexAutoSlip->setEnabled(editable);
	p_vortexFastMoving->setValue(fastMoving);
	p_vortexFastMoving->setEnabled(editable);
	p_vortexPureAsConvex->setValue(treatPureShapesAsConvexShapes);
	p_vortexPureAsConvex->setEnabled(editable);
	p_vortexConvexAsRandom->setValue(treatConvexShapesAsRandomShapes);
	p_vortexConvexAsRandom->setEnabled(editable);
	p_vortexRandomAsTerrain->setValue(treatRandomShapesAsTerrain);
	p_vortexRandomAsTerrain->setEnabled(editable);
	p_autoSleepLinSpeedThreshold->setValue(tt::floatToString(autoSleep_linear_speed_threshold,false).c_str());
	p_autoSleepLinSpeedThreshold->setEnabled(editable);
	p_autoSleepLinAccelThreshold->setValue(tt::floatToString(autoSleep_linear_accel_threshold,false).c_str());
	p_autoSleepLinAccelThreshold->setEnabled(editable);
	p_autoSleepAngSpeedThreshold->setValue(tt::floatToString(autoSleep_angular_speed_threshold,false).c_str());
	p_autoSleepAngSpeedThreshold->setEnabled(editable);
	p_autoSleepAngAccelThreshold->setValue(tt::floatToString(autoSleep_angular_accel_threshold,false).c_str());
	p_autoSleepAngAccelThreshold->setEnabled(editable);
	p_autoSleepStepsThreshold->setValue(tt::intToString(autoSleepStepLiveThreshold).c_str());
	p_autoSleepStepsThreshold->setEnabled(editable);

	p_vortexFrictionModelLinPrim->setValue(frictionModel_primary_linearAxis);
	p_vortexFrictionModelLinPrim->setEnabled(editable);
	p_vortexFrictionCoeffLinPrim->setValue(tt::floatToString(frictionCoeff_primary_linearAxis,false).c_str());
	p_vortexFrictionCoeffLinPrim->setEnabled(editable);
	p_vortexStaticFrictionScaleLinPrim->setValue(tt::floatToString(staticFrictionScale_primary_linearAxis,false).c_str());
	p_vortexStaticFrictionScaleLinPrim->setEnabled(editable);
	p_vortexSlipLinPrim->setValue(tt::floatToString(slip_primary_linearAxis,false).c_str());
	p_vortexSlipLinPrim->setEnabled(editable);
	p_vortexSlideLinPrim->setValue(tt::floatToString(slide_primary_linearAxis,false).c_str());
	p_vortexSlideLinPrim->setEnabled(editable);

	p_primAxisVectorX->setValue(tt::floatToString(primaryAxisVectorX,true).c_str());
	p_primAxisVectorX->setEnabled(editable);
	p_primAxisVectorY->setValue(tt::floatToString(primaryAxisVectorY,true).c_str());
	p_primAxisVectorY->setEnabled(editable);
	p_primAxisVectorZ->setValue(tt::floatToString(primaryAxisVectorZ,true).c_str());
	p_primAxisVectorZ->setEnabled(editable);

	linAxisVectorText+=tt::floatToString(primaryAxisVectorX,true);
	linAxisVectorText+=", ";
	linAxisVectorText+=tt::floatToString(primaryAxisVectorY,true);
	linAxisVectorText+=", ";
	linAxisVectorText+=tt::floatToString(primaryAxisVectorZ,true);
	linAxisVectorText+="]";

	if (frictionModel_primary_linearAxis==6)
		linAxis1Text+=tt::floatToString(0.0f,false);
	else
		linAxis1Text+=tt::floatToString(frictionCoeff_primary_linearAxis,false);
	linAxis1Text+=")";

	p_vortexLinSecFollowsLinPrim->setValue(linearSecFollowsLinearPrim);
	p_vortexLinSecFollowsLinPrim->setEnabled(editable);
	p_vortexFrictionModelLinSec->setValue(frictionModel_secondary_linearAxis);
	p_vortexFrictionModelLinSec->setEnabled((!linearSecFollowsLinearPrim)&&editable);
	p_vortexFrictionCoeffLinSec->setValue(tt::floatToString(frictionCoeff_secondary_linearAxis,false).c_str());
	p_vortexFrictionCoeffLinSec->setEnabled((!linearSecFollowsLinearPrim)&&editable);
	p_vortexStaticFrictionScaleLinSec->setValue(tt::floatToString(staticFrictionScale_secondary_linearAxis,false).c_str());
	p_vortexStaticFrictionScaleLinSec->setEnabled((!linearSecFollowsLinearPrim)&&editable);
	p_vortexSlipLinSec->setValue(tt::floatToString(slip_secondary_linearAxis,false).c_str());
	p_vortexSlipLinSec->setEnabled((!linearSecFollowsLinearPrim)&&editable);
	p_vortexSlideLinSec->setValue(tt::floatToString(slide_secondary_linearAxis,false).c_str());
	p_vortexSlideLinSec->setEnabled((!linearSecFollowsLinearPrim)&&editable);

	if (frictionModel_secondary_linearAxis==6)
		linAxis2Text+=tt::floatToString(0.0f,false);
	else
		linAxis2Text+=tt::floatToString(frictionCoeff_secondary_linearAxis,false);
	linAxis2Text+=")";

	p_vortexFrictionModelAngPrim->setValue(frictionModel_primary_angularAxis);
	p_vortexFrictionModelAngPrim->setEnabled(editable);
	p_vortexFrictionCoeffAngPrim->setValue(tt::floatToString(frictionCoeff_primary_angularAxis,false).c_str());
	p_vortexFrictionCoeffAngPrim->setEnabled(editable);
	p_vortexStaticFrictionScaleAngPrim->setValue(tt::floatToString(staticFrictionScale_primary_angularAxis,false).c_str());
	p_vortexStaticFrictionScaleAngPrim->setEnabled(editable);
	p_vortexSlipAngPrim->setValue(tt::floatToString(slip_primary_angularAxis,false).c_str());
	p_vortexSlipAngPrim->setEnabled(editable);
	p_vortexSlideAngPrim->setValue(tt::floatToString(slide_primary_angularAxis,false).c_str());
	p_vortexSlideAngPrim->setEnabled(editable);

	if (frictionModel_primary_angularAxis==6)
		angAxis1Text+=tt::floatToString(0.0f,false);
	else
		angAxis1Text+=tt::floatToString(frictionCoeff_primary_angularAxis,false);
	angAxis1Text+=")";



	p_vortexAngSecFollowsAngPrim->setValue(angularSecFollowsAngularPrim);
	p_vortexAngSecFollowsAngPrim->setEnabled(editable);
	p_vortexFrictionModelAngSec->setValue(frictionModel_secondary_angularAxis);
	p_vortexFrictionModelAngSec->setEnabled((!angularSecFollowsAngularPrim)&&editable);
	p_vortexFrictionCoeffAngSec->setValue(tt::floatToString(frictionCoeff_secondary_angularAxis,false).c_str());
	p_vortexFrictionCoeffAngSec->setEnabled((!angularSecFollowsAngularPrim)&&editable);
	p_vortexStaticFrictionScaleAngSec->setValue(tt::floatToString(staticFrictionScale_secondary_angularAxis,false).c_str());
	p_vortexStaticFrictionScaleAngSec->setEnabled((!angularSecFollowsAngularPrim)&&editable);
	p_vortexSlipAngSec->setValue(tt::floatToString(slip_secondary_angularAxis,false).c_str());
	p_vortexSlipAngSec->setEnabled((!angularSecFollowsAngularPrim)&&editable);
	p_vortexSlideAngSec->setValue(tt::floatToString(slide_secondary_angularAxis,false).c_str());
	p_vortexSlideAngSec->setEnabled((!angularSecFollowsAngularPrim)&&editable);

	if (frictionModel_secondary_angularAxis==6)
		angAxis2Text+=tt::floatToString(0.0f,false);
	else
		angAxis2Text+=tt::floatToString(frictionCoeff_secondary_angularAxis,false);
	angAxis2Text+=")";

	p_vortexAngNormFollowsAngPrim->setValue(angularNormFollowsAngularPrim);
	p_vortexAngNormFollowsAngPrim->setEnabled(editable);
	p_vortexFrictionModelAngNorm->setValue(frictionModel_normal_angularAxis);
	p_vortexFrictionModelAngNorm->setEnabled((!angularNormFollowsAngularPrim)&&editable);
	p_vortexFrictionCoeffAngNorm->setValue(tt::floatToString(frictionCoeff_normal_angularAxis,false).c_str());
	p_vortexFrictionCoeffAngNorm->setEnabled((!angularNormFollowsAngularPrim)&&editable);
	p_vortexStaticFrictionScaleAngNorm->setValue(tt::floatToString(staticFrictionScale_normal_angularAxis,false).c_str());
	p_vortexStaticFrictionScaleAngNorm->setEnabled((!angularNormFollowsAngularPrim)&&editable);
	p_vortexSlipAngNorm->setValue(tt::floatToString(slip_normal_angularAxis,false).c_str());
	p_vortexSlipAngNorm->setEnabled((!angularNormFollowsAngularPrim)&&editable);
	p_vortexSlideAngNorm->setValue(tt::floatToString(slide_normal_angularAxis,false).c_str());
	p_vortexSlideAngNorm->setEnabled((!angularNormFollowsAngularPrim)&&editable);

	if (frictionModel_normal_angularAxis==6)
		angAxis3Text+=tt::floatToString(0.0f,false);
	else
		angAxis3Text+=tt::floatToString(frictionCoeff_normal_angularAxis,false);
	angAxis3Text+=")";

	// some group labels:
	linAxisPrim->setPropertyName(linAxis1Text.c_str());
	linAxisSec->setPropertyName(linAxis2Text.c_str());
	angAxisPrim->setPropertyName(angAxis1Text.c_str());
	angAxisSec->setPropertyName(angAxis2Text.c_str());
	angAxisNorm->setPropertyName(angAxis3Text.c_str());

	linAxisPrimVector->setPropertyName(linAxisVectorText.c_str());

}

QtBrowserItem* CPropBrowserEngineMaterial::getSubPropertyBrowserItem(const QtBrowserItem* parentBrowserItem,const QtProperty* childProperty)
{
	QList<QtBrowserItem*> children=parentBrowserItem->children();
	for (int i=0;i<int(children.size());i++)
	{
		if (children[i]->property()==childProperty)
			return(children[i]);
	}
	return(NULL);
}


void CPropBrowserEngineMaterial::catchPropertyChanges(QtProperty *_prop, QVariant value)
{
	enableNotifications(false);
	handlePropertyChanges(_prop);
	enableNotifications(true);
}
void CPropBrowserEngineMaterial::catchPropertyChangesString(QtProperty *_prop, QString value)
{
	enableNotifications(false);
	handlePropertyChanges(_prop);
	enableNotifications(true);
}

void CPropBrowserEngineMaterial::handlePropertyChanges(QtProperty *_prop)
{
	CShape* it=(CShape*)App::ct->objCont->getLastSelection();
	CGeomWrap* geom=it->geomData->geomInfo;
	CDynMaterialObject* mat=geom->getDynMaterialObject();

	float f;
	int a;

	// Did we change the material name?
	if (_prop==p_materialName)
	{
		std::string name(((QtVariantProperty*)_prop)->value().toString().toStdString());
		if (name.compare(0,4,"usr_")!=0)
			name="usr_"+name;
		while (App::ct->dynMaterialCont->getObject(name.c_str())!=NULL)
			name=tt::generateNewName_noDash(name);
		mat->setObjectName(name.c_str());
	}

	// Did we click the duplicate button?
	if (_prop==p_duplicateButton)
	{
		CDynMaterialObject* copy=mat->copyYourself();
		std::string name(copy->getObjectName());
		if (name.compare(0,4,"usr_")!=0)
			name="usr_"+name;
		copy->setObjectName(name.c_str());
		int newMatId=App::ct->dynMaterialCont->addObject(copy,true);
		geom->setDynMaterialId(newMatId);
		mat=copy;
	}

	// Bullet parameters:
	if (_prop==p_bulletFriction)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			mat->setBulletFriction(f);
	}
	if (_prop==p_bulletRestitution)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			mat->setBulletRestitution(f);
	}
	if (_prop==p_bulletLinDamping)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			mat->setBulletLinearDamping(f);
	}
	if (_prop==p_bulletAngDamping)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			mat->setBulletAngularDamping(f);
	}
	if (_prop==p_bulletStickyContact)
		mat->setBulletStickyContact(((QtVariantProperty*)_prop)->value().toBool());
	if (_prop==p_bulletAutoShrink)
		mat->setBulletAutoShrinkConvexMesh(((QtVariantProperty*)_prop)->value().toBool());
	if (_prop==p_bulletCustomMarginScaling)
	{
		bool forConvexParam=(geom->isConvex()&&(!geom->isPure()));
		mat->setBulletNonDefaultCollisionMargin(forConvexParam,((QtVariantProperty*)_prop)->value().toBool());
	}
	if (_prop==p_bulletMarginScalingFactor)
	{
		bool forConvexParam=(geom->isConvex()&&(!geom->isPure()));
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			mat->setBulletNonDefaultCollisionMarginFactor(forConvexParam,f);
	}

	// ODE parameters:
	if (_prop==p_odeFriction)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			mat->setOdeFriction(f);
	}
	if (_prop==p_odeMaxContacts)
	{
		if (tt::stringToInt(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),a))
			mat->setOdeMaxContacts(a);
	}
	if (_prop==p_odeSoftERP)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			mat->setOdeSoftERP(f);
	}
	if (_prop==p_odeSoftCFM)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			mat->setOdeSoftCFM(f);
	}
	if (_prop==p_odeLinDamping)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			mat->setOdeLinearDamping(f);
	}
	if (_prop==p_odeAngDamping)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			mat->setOdeAngularDamping(f);
	}

	// Vortex parameters:
	std::vector<float> floatParams;
	std::vector<int> intParams;
	mat->getVortexFloatParams(floatParams);
	mat->getVortexIntParams(intParams);
	float frictionCoeff_primary_linearAxis=floatParams[0];
	float frictionCoeff_secondary_linearAxis=floatParams[1];
	float frictionCoeff_primary_angularAxis=floatParams[2];
	float frictionCoeff_secondary_angularAxis=floatParams[3];
	float frictionCoeff_normal_angularAxis=floatParams[4];
	float staticFrictionScale_primary_linearAxis=floatParams[5];
	float staticFrictionScale_secondary_linearAxis=floatParams[6];
	float staticFrictionScale_primary_angularAxis=floatParams[7];
	float staticFrictionScale_secondary_angularAxis=floatParams[8];
	float staticFrictionScale_normal_angularAxis=floatParams[9];
	float compliance=floatParams[10];
	float damping=floatParams[11];
	float restitution=floatParams[12];
	float restitutionThreshold=floatParams[13];
	float adhesiveForce=floatParams[14];
	float linearVelocityDamping=floatParams[15];
	float angularVelocityDamping=floatParams[16];
	float slide_primary_linearAxis=floatParams[17];
	float slide_secondary_linearAxis=floatParams[18];
	float slide_primary_angularAxis=floatParams[19];
	float slide_secondary_angularAxis=floatParams[20];
	float slide_normal_angularAxis=floatParams[21];
	float slip_primary_linearAxis=floatParams[22];
	float slip_secondary_linearAxis=floatParams[23];
	float slip_primary_angularAxis=floatParams[24];
	float slip_secondary_angularAxis=floatParams[25];
	float slip_normal_angularAxis=floatParams[26];
	float autoSleep_linear_speed_threshold=floatParams[27];
	float autoSleep_linear_accel_threshold=floatParams[28];
	float autoSleep_angular_speed_threshold=floatParams[29];
	float autoSleep_angular_accel_threshold=floatParams[30];
	float skin_thickness=floatParams[31];
	float autoAngularDampingTensionRatio=floatParams[32];
	float primaryAxisVectorX=floatParams[33];
	float primaryAxisVectorY=floatParams[34];
	float primaryAxisVectorZ=floatParams[35];

	int frictionModel_primary_linearAxis=intParams[0];
	int frictionModel_secondary_linearAxis=intParams[1];
	int frictionModel_primary_angularAxis=intParams[2];
	int frictionModel_secondary_angularAxis=intParams[3];
	int frictionModel_normal_angularAxis=intParams[4];
	bool treatPureShapesAsConvexShapes=((intParams[5]&1)!=0);
	bool treatConvexShapesAsRandomShapes=((intParams[5]&2)!=0);
	bool treatRandomShapesAsTerrain=((intParams[5]&4)!=0);
	bool fastMoving=((intParams[5]&8)!=0);
	bool autoSlip=((intParams[5]&16)!=0);
	bool linearSecFollowsLinearPrim=((intParams[5]&32)!=0);
	bool angularSecFollowsAngularPrim=((intParams[5]&64)!=0);
	bool angularNormFollowsAngularPrim=((intParams[5]&128)!=0);
	bool autoAngularDampingEnabled=((intParams[5]&256)!=0);
	int autoSleepStepLiveThreshold=intParams[6];
	int uniqueID=intParams[7];


	if (_prop==p_vortexRestitution)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			restitution=f;
	}
	if (_prop==p_vortexRestitutionThreshold)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			restitutionThreshold=f;
	}
	if (_prop==p_vortexCompliance)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			compliance=tt::userFloatToFloat(f,gv::userToSeconds*gv::userToSeconds/gv::userToKilograms,true);
	}
	if (_prop==p_vortexDamping)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			damping=tt::userFloatToFloat(f,gv::userToKilograms/gv::userToSeconds,true);
	}
	if (_prop==p_vortexAdhesiveForce)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			adhesiveForce=tt::userFloatToFloat(f,gv::userToForce,true);
	}
	if (_prop==p_vortexLinVelocityDamping)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			linearVelocityDamping=tt::userFloatToFloat(f,gv::userToKilograms/gv::userToSeconds,true);
	}
	if (_prop==p_vortexAngVelocityDamping)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			angularVelocityDamping=tt::userFloatToFloat(f,gv::userToKilograms*gv::userToMeter*gv::userToMeter/gv::userToSeconds,true);
	}
	if (_prop==p_vortexAutoAngularDampingEnabled)
		autoAngularDampingEnabled=((QtVariantProperty*)_prop)->value().toBool();
	if (_prop==p_vortexAutoAngularDampingTensionRatio)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			autoAngularDampingTensionRatio=f;
	}
	if (_prop==p_vortexSkinThickness)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			skin_thickness=tt::userFloatToFloat(f,gv::userToMeter,false);
	}
	if (_prop==p_vortexAutoSlip)
		autoSlip=((QtVariantProperty*)_prop)->value().toBool();
	if (_prop==p_vortexFastMoving)
		fastMoving=((QtVariantProperty*)_prop)->value().toBool();
	if (_prop==p_vortexPureAsConvex)
		treatPureShapesAsConvexShapes=((QtVariantProperty*)_prop)->value().toBool();
	if (_prop==p_vortexConvexAsRandom)
		treatConvexShapesAsRandomShapes=((QtVariantProperty*)_prop)->value().toBool();
	if (_prop==p_vortexRandomAsTerrain)
		treatRandomShapesAsTerrain=((QtVariantProperty*)_prop)->value().toBool();
	if (_prop==p_autoSleepLinSpeedThreshold)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			autoSleep_linear_speed_threshold=tt::userFloatToFloat(f,gv::userToLinearVel,true);
	}
	if (_prop==p_autoSleepLinAccelThreshold)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			autoSleep_linear_accel_threshold=tt::userFloatToFloat(f,gv::userToLinearAccel,true);
	}
	if (_prop==p_autoSleepAngSpeedThreshold)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			autoSleep_angular_speed_threshold=tt::userFloatToFloat(f,gv::userToAngularVel,true);
	}
	if (_prop==p_autoSleepAngAccelThreshold)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			autoSleep_angular_accel_threshold=tt::userFloatToFloat(f,gv::userToAngularAccel,true);
	}
	if (_prop==p_autoSleepStepsThreshold)
	{
		if (tt::stringToInt(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),a))
			autoSleepStepLiveThreshold=a;
	}


	if (_prop==p_primAxisVectorX)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,true,false))
			primaryAxisVectorX=tt::userFloatToFloat(f,1,false);
	}

	if (_prop==p_primAxisVectorY)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,true,false))
			primaryAxisVectorY=tt::userFloatToFloat(f,1,false);
	}

	if (_prop==p_primAxisVectorZ)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,true,false))
			primaryAxisVectorZ=tt::userFloatToFloat(f,1,false);
	}


	if (_prop==p_vortexFrictionModelLinPrim)
		frictionModel_primary_linearAxis=((QtVariantProperty*)_prop)->value().toInt();
	if (_prop==p_vortexFrictionCoeffLinPrim)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			frictionCoeff_primary_linearAxis=f;
	}
	if (_prop==p_vortexStaticFrictionScaleLinPrim)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			staticFrictionScale_primary_linearAxis=f;
	}
	if (_prop==p_vortexSlipLinPrim)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			slip_primary_linearAxis=tt::userFloatToFloat(f,gv::userToSeconds/gv::userToKilograms,true);
	}
	if (_prop==p_vortexSlideLinPrim)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			slide_primary_linearAxis=tt::userFloatToFloat(f,gv::userToLinearVel,true);
	}


	if (_prop==p_vortexLinSecFollowsLinPrim)
		linearSecFollowsLinearPrim=((QtVariantProperty*)_prop)->value().toBool();
	if (_prop==p_vortexFrictionModelLinSec)
		frictionModel_secondary_linearAxis=((QtVariantProperty*)_prop)->value().toInt();
	if (_prop==p_vortexFrictionCoeffLinSec)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			frictionCoeff_secondary_linearAxis=f;
	}
	if (_prop==p_vortexStaticFrictionScaleLinSec)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			staticFrictionScale_secondary_linearAxis=f;
	}
	if (_prop==p_vortexSlipLinSec)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			slip_secondary_linearAxis=tt::userFloatToFloat(f,gv::userToSeconds/gv::userToKilograms,true);
	}
	if (_prop==p_vortexSlideLinSec)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			slide_secondary_linearAxis=tt::userFloatToFloat(f,gv::userToLinearVel,true);
	}


	if (_prop==p_vortexFrictionModelAngPrim)
		frictionModel_primary_angularAxis=((QtVariantProperty*)_prop)->value().toInt();
	if (_prop==p_vortexFrictionCoeffAngPrim)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			frictionCoeff_primary_angularAxis=f;
	}
	if (_prop==p_vortexStaticFrictionScaleAngPrim)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			staticFrictionScale_primary_angularAxis=f;
	}
	if (_prop==p_vortexSlipAngPrim)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			slip_primary_angularAxis=tt::userFloatToFloat(f,gv::userToSeconds/(gv::userToKilograms*gv::userToMeter*gv::userToMeter),true);
	}
	if (_prop==p_vortexSlideAngPrim)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			slide_primary_angularAxis=tt::userFloatToFloat(f,gv::userToAngularVel,true);
	}


	if (_prop==p_vortexAngSecFollowsAngPrim)
		angularSecFollowsAngularPrim=((QtVariantProperty*)_prop)->value().toBool();
	if (_prop==p_vortexFrictionModelAngSec)
		frictionModel_secondary_angularAxis=((QtVariantProperty*)_prop)->value().toInt();
	if (_prop==p_vortexFrictionCoeffAngSec)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			frictionCoeff_secondary_angularAxis=f;
	}
	if (_prop==p_vortexStaticFrictionScaleAngSec)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			staticFrictionScale_secondary_angularAxis=f;
	}
	if (_prop==p_vortexSlipAngSec)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			slip_secondary_angularAxis=tt::userFloatToFloat(f,gv::userToSeconds/(gv::userToKilograms*gv::userToMeter*gv::userToMeter),true);
	}
	if (_prop==p_vortexSlideAngSec)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			slide_secondary_angularAxis=tt::userFloatToFloat(f,gv::userToAngularVel,true);
	}


	if (_prop==p_vortexAngNormFollowsAngPrim)
		angularNormFollowsAngularPrim=((QtVariantProperty*)_prop)->value().toBool();
	if (_prop==p_vortexFrictionModelAngNorm)
		frictionModel_normal_angularAxis=((QtVariantProperty*)_prop)->value().toInt();
	if (_prop==p_vortexFrictionCoeffAngNorm)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			frictionCoeff_normal_angularAxis=f;
	}
	if (_prop==p_vortexStaticFrictionScaleAngNorm)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			staticFrictionScale_normal_angularAxis=f;
	}
	if (_prop==p_vortexSlipAngNorm)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			slip_normal_angularAxis=tt::userFloatToFloat(f,gv::userToSeconds/(gv::userToKilograms*gv::userToMeter*gv::userToMeter),true);
	}
	if (_prop==p_vortexSlideAngNorm)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,true))
			slide_normal_angularAxis=tt::userFloatToFloat(f,gv::userToAngularVel,true);
	}




	//************************************
	if (linearSecFollowsLinearPrim)
	{
		frictionCoeff_secondary_linearAxis=frictionCoeff_primary_linearAxis;
		staticFrictionScale_secondary_linearAxis=staticFrictionScale_primary_linearAxis;
		slide_secondary_linearAxis=slide_primary_linearAxis;
		slip_secondary_linearAxis=slip_primary_linearAxis;
		frictionModel_secondary_linearAxis=frictionModel_primary_linearAxis;
	}
	if (angularSecFollowsAngularPrim)
	{
		frictionCoeff_secondary_angularAxis=frictionCoeff_primary_angularAxis;
		staticFrictionScale_secondary_angularAxis=staticFrictionScale_primary_angularAxis;
		slide_secondary_angularAxis=slide_primary_angularAxis;
		slip_secondary_angularAxis=slip_primary_angularAxis;
		frictionModel_secondary_angularAxis=frictionModel_primary_angularAxis;
	}
	if (angularNormFollowsAngularPrim)
	{
		frictionCoeff_normal_angularAxis=frictionCoeff_primary_angularAxis;
		staticFrictionScale_normal_angularAxis=staticFrictionScale_primary_angularAxis;
		slide_normal_angularAxis=slide_primary_angularAxis;
		slip_normal_angularAxis=slip_primary_angularAxis;
		frictionModel_normal_angularAxis=frictionModel_primary_angularAxis;
	}
	//************************************

	floatParams[0]=frictionCoeff_primary_linearAxis;
	floatParams[1]=frictionCoeff_secondary_linearAxis;
	floatParams[2]=frictionCoeff_primary_angularAxis;
	floatParams[3]=frictionCoeff_secondary_angularAxis;
	floatParams[4]=frictionCoeff_normal_angularAxis;
	floatParams[5]=staticFrictionScale_primary_linearAxis;
	floatParams[6]=staticFrictionScale_secondary_linearAxis;
	floatParams[7]=staticFrictionScale_primary_angularAxis;
	floatParams[8]=staticFrictionScale_secondary_angularAxis;
	floatParams[9]=staticFrictionScale_normal_angularAxis;
	floatParams[10]=compliance;
	floatParams[11]=damping;
	floatParams[12]=restitution;
	floatParams[13]=restitutionThreshold;
	floatParams[14]=adhesiveForce;
	floatParams[15]=linearVelocityDamping;
	floatParams[16]=angularVelocityDamping;
	floatParams[17]=slide_primary_linearAxis;
	floatParams[18]=slide_secondary_linearAxis;
	floatParams[19]=slide_primary_angularAxis;
	floatParams[20]=slide_secondary_angularAxis;
	floatParams[21]=slide_normal_angularAxis;
	floatParams[22]=slip_primary_linearAxis;
	floatParams[23]=slip_secondary_linearAxis;
	floatParams[24]=slip_primary_angularAxis;
	floatParams[25]=slip_secondary_angularAxis;
	floatParams[26]=slip_normal_angularAxis;
	floatParams[27]=autoSleep_linear_speed_threshold;
	floatParams[28]=autoSleep_linear_accel_threshold;
	floatParams[29]=autoSleep_angular_speed_threshold;
	floatParams[30]=autoSleep_angular_accel_threshold;
	floatParams[31]=skin_thickness;
	floatParams[32]=autoAngularDampingTensionRatio;
	floatParams[33]=primaryAxisVectorX;
	floatParams[34]=primaryAxisVectorY;
	floatParams[35]=primaryAxisVectorZ;
	intParams[0]=frictionModel_primary_linearAxis;
	intParams[1]=frictionModel_secondary_linearAxis;
	intParams[2]=frictionModel_primary_angularAxis;
	intParams[3]=frictionModel_secondary_angularAxis;
	intParams[4]=frictionModel_normal_angularAxis;
	intParams[5]=0;
	if (treatPureShapesAsConvexShapes)
		intParams[5]|=1;
	if (treatConvexShapesAsRandomShapes)
		intParams[5]|=2;
	if (treatRandomShapesAsTerrain)
		intParams[5]|=4;
	if (fastMoving)
		intParams[5]|=8;
	if (autoSlip)
		intParams[5]|=16;
	if (linearSecFollowsLinearPrim)
		intParams[5]|=32;
	if (angularSecFollowsAngularPrim)
		intParams[5]|=64;
	if (angularNormFollowsAngularPrim)
		intParams[5]|=128;
	if (autoAngularDampingEnabled)
		intParams[5]|=256;
	intParams[6]=autoSleepStepLiveThreshold;
	intParams[7]=uniqueID; // not important


	mat->setVortexFloatParams(floatParams);
	mat->setVortexIntParams(intParams);
	refresh();
}
