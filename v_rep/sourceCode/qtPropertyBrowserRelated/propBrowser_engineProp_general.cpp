#include "propBrowser_engineProp_general.h"

#include "Tt.h"
#include "GV.h"
#include "App.h"
#include "IloIlo.h"
#include "v_repStrings.h"


bool CPropBrowserEngineGeneral::_bulletPropertiesExpanded=true;
bool CPropBrowserEngineGeneral::_odePropertiesExpanded=true;
bool CPropBrowserEngineGeneral::_vortexPropertiesExpanded=true;
bool CPropBrowserEngineGeneral::_vortexConstraintPropertiesExpanded=false;
int CPropBrowserEngineGeneral::_dlgSizeX=500;
int CPropBrowserEngineGeneral::_dlgSizeY=480;
int CPropBrowserEngineGeneral::_dlgPosX=800;
int CPropBrowserEngineGeneral::_dlgPosY=100;
int CPropBrowserEngineGeneral::_splitterPos=250;

CPropBrowserEngineGeneral::CPropBrowserEngineGeneral(QWidget* pParent) : QtTreePropertyBrowser(pParent)
{
	show(NULL);
}

CPropBrowserEngineGeneral::~CPropBrowserEngineGeneral()
{
	enableNotifications(false);

	_bulletPropertiesExpanded=isExpanded(bulletGroupItem);
	_odePropertiesExpanded=isExpanded(odeGroupItem);
	_vortexPropertiesExpanded=isExpanded(vortexGroupItem);
	_vortexConstraintPropertiesExpanded=isExpanded(vortexConstraintItem);

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


void CPropBrowserEngineGeneral::reject()
{
	done(QDialog::Accepted);
}

void CPropBrowserEngineGeneral::accept()
{
	done(QDialog::Accepted);
}


void CPropBrowserEngineGeneral::show(QWidget* parentWindow)
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
	setWindowTitle("Physics Engines Properties - General");


	variantManager = new QtVariantPropertyManager();
	variantFactory = new QtVariantEditorFactory();
	setFactoryForManager(variantManager,variantFactory);


	buttonManager = new ButtonEditManager();
	buttonFactory  = new PushButtonEditFactory();
	setFactoryForManager(buttonManager,buttonFactory);


	QStringList configurationEnum;
	configurationEnum << "Very accurate" << "Accurate (default)" << "Fast" << "Very fast" << "Customized";
	p_configuration = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),"");
	p_configuration->setAttribute("enumNames", configurationEnum);
	QtBrowserItem* anItem=addProperty(p_configuration);
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

	// Bullet:
	p_bulletTimeStep = variantManager->addProperty(QVariant::String,"");
	bulletGroup->addSubProperty(p_bulletTimeStep);
	p_bulletConstraintSolvIterat = variantManager->addProperty(QVariant::String,"");
	bulletGroup->addSubProperty(p_bulletConstraintSolvIterat);
	p_bulletInternalScaling = variantManager->addProperty(QVariant::String,"");
	bulletGroup->addSubProperty(p_bulletInternalScaling);
	p_bulletInternalFullScaling = variantManager->addProperty(QVariant::Bool,"");
	bulletGroup->addSubProperty(p_bulletInternalFullScaling);
	p_bulletCollMarginScaling = variantManager->addProperty(QVariant::String,"");
	bulletGroup->addSubProperty(p_bulletCollMarginScaling);

	// ODE:
	p_odeTimeStep = variantManager->addProperty(QVariant::String,"");
	odeGroup->addSubProperty(p_odeTimeStep);
	p_odeQuickStep = variantManager->addProperty(QVariant::Bool,"");
	odeGroup->addSubProperty(p_odeQuickStep);
	p_odeIterations = variantManager->addProperty(QVariant::String,"");
	odeGroup->addSubProperty(p_odeIterations);
	p_odeInternalScaling = variantManager->addProperty(QVariant::String,"");
	odeGroup->addSubProperty(p_odeInternalScaling);
	p_odeInternalFullScaling = variantManager->addProperty(QVariant::Bool,"");
	odeGroup->addSubProperty(p_odeInternalFullScaling);
	p_odeGlobalErp = variantManager->addProperty(QVariant::String,"");
	odeGroup->addSubProperty(p_odeGlobalErp);
	p_odeGlobalCfm = variantManager->addProperty(QVariant::String,"");
	odeGroup->addSubProperty(p_odeGlobalCfm);

	// Vortex:
	p_vortexTimeStep = variantManager->addProperty(QVariant::String,"");
	vortexGroup->addSubProperty(p_vortexTimeStep);
	p_vortexContactTolerance = variantManager->addProperty(QVariant::String,"");
	vortexGroup->addSubProperty(p_vortexContactTolerance);
	p_vortexInternalScaling = variantManager->addProperty(QVariant::String,"");
	vortexGroup->addSubProperty(p_vortexInternalScaling);
	p_vortexInternalFullScaling = variantManager->addProperty(QVariant::Bool,"");
	vortexGroup->addSubProperty(p_vortexInternalFullScaling);
	p_vortexAutoSleep = variantManager->addProperty(QVariant::Bool,"");
	vortexGroup->addSubProperty(p_vortexAutoSleep);
	p_vortexMultithreading = variantManager->addProperty(QVariant::Bool,"");
	vortexGroup->addSubProperty(p_vortexMultithreading);

	QtProperty* vortexConstraint = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"Constraint properties");
	vortexGroup->addSubProperty(vortexConstraint);
	vortexConstraintItem=getSubPropertyBrowserItem(vortexGroupItem,vortexConstraint);
	setExpanded(vortexConstraintItem,_vortexConstraintPropertiesExpanded);

	p_vortexConstraintLinearCompliance = variantManager->addProperty(QVariant::String,"");
	vortexConstraint->addSubProperty(p_vortexConstraintLinearCompliance);
	p_vortexConstraintLinearDamping = variantManager->addProperty(QVariant::String,"");
	vortexConstraint->addSubProperty(p_vortexConstraintLinearDamping);
	p_vortexConstraintLinearKinLoss = variantManager->addProperty(QVariant::String,"");
	vortexConstraint->addSubProperty(p_vortexConstraintLinearKinLoss);
	p_vortexConstraintAngularCompliance = variantManager->addProperty(QVariant::String,"");
	vortexConstraint->addSubProperty(p_vortexConstraintAngularCompliance);
	p_vortexConstraintAngularDamping = variantManager->addProperty(QVariant::String,"");
	vortexConstraint->addSubProperty(p_vortexConstraintAngularDamping);
	p_vortexConstraintAngularKinLoss = variantManager->addProperty(QVariant::String,"");
	vortexConstraint->addSubProperty(p_vortexConstraintAngularKinLoss);


	refresh();

	enableNotifications(true);

//	setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint);
//	setWindowModality(Qt::ApplicationModal);
	setGeometry(_dlgPosX,_dlgPosY,_dlgSizeX,_dlgSizeY);

}

void CPropBrowserEngineGeneral::enableNotifications(bool enable)
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

void CPropBrowserEngineGeneral::refresh()
{
	int confIndex=App::ct->dynamicsContainer->getUseDynamicDefaultCalculationParameters();

	// Configurationj drop box:
	p_configuration->setPropertyName("Configuration");
	p_configuration->setValue(confIndex);

	// Bullet property names:
	p_bulletTimeStep->setPropertyName(gv::formatUnitStr("Bullet time step","s").c_str());
	p_bulletConstraintSolvIterat->setPropertyName("Constraint solving iterations");
	p_bulletInternalScaling->setPropertyName("Internal scaling");
	p_bulletInternalFullScaling->setPropertyName("Full scaling");
	p_bulletCollMarginScaling->setPropertyName("Collision margin scaling");

	// Bullet parameters:
	p_bulletTimeStep->setValue(tt::floatToString(App::ct->dynamicsContainer->getDynamicsStepSize()*gv::secondsToUser,false).c_str());
	p_bulletTimeStep->setEnabled(confIndex==4);
	p_bulletConstraintSolvIterat->setValue(tt::intToString(App::ct->dynamicsContainer->getDynamicConstraintSolvingIterations()).c_str());
	p_bulletConstraintSolvIterat->setEnabled(confIndex==4);
	p_bulletInternalScaling->setValue(tt::floatToString(App::ct->dynamicsContainer->getDynamicInternalScalingFactor(),false).c_str());
	p_bulletInternalScaling->setEnabled(confIndex==4);
	p_bulletInternalFullScaling->setValue(App::ct->dynamicsContainer->getBulletDynamicFullInternalScaling());
	p_bulletInternalFullScaling->setEnabled(confIndex==4);
	p_bulletCollMarginScaling->setValue(tt::floatToString(App::ct->dynamicsContainer->getBulletDynamicCollisionMarginFactor(),false).c_str());
	p_bulletCollMarginScaling->setEnabled(confIndex==4);


	// ODE property names:
	p_odeTimeStep->setPropertyName(gv::formatUnitStr("ODE time step","s").c_str());
	p_odeQuickStep->setPropertyName("Use 'quickStep'");
	p_odeIterations->setPropertyName("QuickStep iterations");
	p_odeInternalScaling->setPropertyName("Internal scaling");
	p_odeInternalFullScaling->setPropertyName("Full scaling");
	p_odeGlobalErp->setPropertyName("Global ERP");
	p_odeGlobalCfm->setPropertyName("Global CFM");

	// ODE parameters:
	p_odeTimeStep->setValue(tt::floatToString(App::ct->dynamicsContainer->getODEDynamicsStepSize()*gv::secondsToUser,false).c_str());
	p_odeTimeStep->setEnabled(confIndex==4);
	p_odeQuickStep->setValue(App::ct->dynamicsContainer->getODEUseQuickStep());
	p_odeQuickStep->setEnabled(confIndex==4);
	p_odeIterations->setValue(tt::intToString(App::ct->dynamicsContainer->getODEDynamicConstraintSolvingIterations()).c_str());
	p_odeIterations->setEnabled((confIndex==4)&&App::ct->dynamicsContainer->getODEUseQuickStep());
	p_odeInternalScaling->setValue(tt::floatToString(App::ct->dynamicsContainer->getODEDynamicInternalScalingFactor(),false).c_str());
	p_odeInternalScaling->setEnabled(confIndex==4);
	p_odeInternalFullScaling->setValue(App::ct->dynamicsContainer->getODEDynamicFullInternalScaling());
	p_odeInternalFullScaling->setEnabled(confIndex==4);
	p_odeGlobalErp->setValue(tt::floatToString(App::ct->dynamicsContainer->getODEDynamicGlobalERP(),false).c_str());
	p_odeGlobalErp->setEnabled(confIndex==4);
	p_odeGlobalCfm->setValue(tt::floatToString(App::ct->dynamicsContainer->getODEDynamicGlobalCFM(),false).c_str());
	p_odeGlobalCfm->setEnabled(confIndex==4);


	// Vortex property names:
	p_vortexTimeStep->setPropertyName(gv::formatUnitStr("Vortex time step","s").c_str());
	p_vortexContactTolerance->setPropertyName(gv::formatUnitStr("Contact tolerance","m").c_str());
	p_vortexInternalScaling->setPropertyName("Internal scaling");
	p_vortexInternalFullScaling->setPropertyName("Full scaling");
	p_vortexAutoSleep->setPropertyName("Auto-sleep");
	p_vortexMultithreading->setPropertyName("Multithreading");

	p_vortexConstraintLinearCompliance->setPropertyName(gv::formatUnitStr("Linear compliance","s^2/k").c_str());
	p_vortexConstraintLinearDamping->setPropertyName(gv::formatUnitStr("Linear damping","k/s").c_str());
	p_vortexConstraintLinearKinLoss->setPropertyName(gv::formatUnitStr("Linear kinetic loss","s/k").c_str());

	p_vortexConstraintAngularCompliance->setPropertyName(gv::formatUnitStr("Angular compliance","s^2/(k*m)").c_str());
	p_vortexConstraintAngularDamping->setPropertyName(gv::formatUnitStr("Angular damping","k*m^2/s").c_str());
	p_vortexConstraintAngularKinLoss->setPropertyName(gv::formatUnitStr("Angular kinetic loss","s/(k*m^2)").c_str());


	// Vortex parameters:
	std::vector<float> floatParams;
	std::vector<int> intParams;
	App::ct->dynamicsContainer->getVortexFloatParams(floatParams);
	App::ct->dynamicsContainer->getVortexIntParams(intParams);

	const float stepSize=floatParams[0]*gv::secondsToUser;
	const float internalScalingFactor=floatParams[1];
	const float contactTolerance=floatParams[2]*gv::meterToUser;
	const float constraint_linear_compliance=tt::floatToUserFloat(floatParams[3],gv::secondsToUser*gv::secondsToUser/gv::kilogramsToUser,true);
	const float constraint_linear_damping=tt::floatToUserFloat(floatParams[4],gv::kilogramsToUser/gv::secondsToUser,true);
	const float constraint_linear_kineticLoss=tt::floatToUserFloat(floatParams[5],gv::secondsToUser/gv::kilogramsToUser,true);
	const float constraint_angular_compliance=tt::floatToUserFloat(floatParams[6],gv::secondsToUser*gv::secondsToUser/(gv::kilogramsToUser*gv::meterToUser),true);
	const float constraint_angular_damping=tt::floatToUserFloat(floatParams[7],gv::kilogramsToUser*gv::meterToUser*gv::meterToUser/gv::secondsToUser,true);
	const float constraint_angular_kineticLoss=tt::floatToUserFloat(floatParams[8],gv::secondsToUser/(gv::kilogramsToUser*gv::meterToUser*gv::meterToUser),true);
	// floatParams[9] IS RESERVED (was auto angular damping tension ratio)
	const bool autoSleep=((intParams[0]&1)!=0);
	const bool multiThreading=((intParams[0]&2)!=0);
	const bool fullInternalScaling=((intParams[0]&4)!=0);
	// ((intParams[0]&8) IS RESERVED (was auto angular damping enabled flag)

	p_vortexTimeStep->setValue(tt::floatToString(stepSize,false).c_str());
	p_vortexTimeStep->setEnabled(confIndex==4);
	p_vortexContactTolerance->setValue(tt::floatToString(contactTolerance,false).c_str());
	p_vortexContactTolerance->setEnabled(confIndex==4);
	p_vortexInternalScaling->setValue(tt::floatToString(internalScalingFactor,false).c_str());
	p_vortexInternalScaling->setEnabled(confIndex==4);
	p_vortexInternalFullScaling->setValue(fullInternalScaling);
	p_vortexInternalFullScaling->setEnabled(confIndex==4);
	p_vortexAutoSleep->setValue(autoSleep);
	p_vortexAutoSleep->setEnabled(confIndex==4);
	p_vortexMultithreading->setValue(multiThreading);
	p_vortexMultithreading->setEnabled(confIndex==4);

	p_vortexConstraintLinearCompliance->setValue(tt::floatToString(constraint_linear_compliance,false).c_str());
	p_vortexConstraintLinearCompliance->setEnabled(confIndex==4);
	p_vortexConstraintLinearDamping->setValue(tt::floatToString(constraint_linear_damping,false).c_str());
	p_vortexConstraintLinearDamping->setEnabled(confIndex==4);
	p_vortexConstraintLinearKinLoss->setValue(tt::floatToString(constraint_linear_kineticLoss,false).c_str());
	p_vortexConstraintLinearKinLoss->setEnabled(confIndex==4);
	p_vortexConstraintAngularCompliance->setValue(tt::floatToString(constraint_angular_compliance,false).c_str());
	p_vortexConstraintAngularCompliance->setEnabled(confIndex==4);
	p_vortexConstraintAngularDamping->setValue(tt::floatToString(constraint_angular_damping,false).c_str());
	p_vortexConstraintAngularDamping->setEnabled(confIndex==4);
	p_vortexConstraintAngularKinLoss->setValue(tt::floatToString(constraint_angular_kineticLoss,false).c_str());
	p_vortexConstraintAngularKinLoss->setEnabled(confIndex==4);
}

QtBrowserItem* CPropBrowserEngineGeneral::getSubPropertyBrowserItem(const QtBrowserItem* parentBrowserItem,const QtProperty* childProperty)
{
	QList<QtBrowserItem*> children=parentBrowserItem->children();
	for (int i=0;i<int(children.size());i++)
	{
		if (children[i]->property()==childProperty)
			return(children[i]);
	}
	return(NULL);
}


void CPropBrowserEngineGeneral::catchPropertyChanges(QtProperty *_prop, QVariant value)
{
	enableNotifications(false);
	handlePropertyChanges(_prop);
	enableNotifications(true);
}
void CPropBrowserEngineGeneral::catchPropertyChangesString(QtProperty *_prop, QString value)
{
	enableNotifications(false);
	handlePropertyChanges(_prop);
	enableNotifications(true);
}

void CPropBrowserEngineGeneral::handlePropertyChanges(QtProperty *_prop)
{
	float f;
	int a;

	// Config drop box:
	if (_prop==p_configuration)
		App::ct->dynamicsContainer->setUseDynamicDefaultCalculationParameters(((QtVariantProperty*)_prop)->value().toInt());

	// Bullet parameters:
	if (_prop==p_bulletTimeStep)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			App::ct->dynamicsContainer->setDynamicsStepSize(f*gv::userToSeconds,true);
	}
	if (_prop==p_bulletConstraintSolvIterat)
	{
		if (tt::stringToInt(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),a))
			App::ct->dynamicsContainer->setDynamicConstraintSolvingIterations(a,true);
	}
	if (_prop==p_bulletInternalScaling)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			App::ct->dynamicsContainer->setDynamicInternalScalingFactor(f,true);
	}
	if (_prop==p_bulletInternalFullScaling)
		App::ct->dynamicsContainer->setBulletDynamicFullInternalScaling(((QtVariantProperty*)_prop)->value().toBool(),true);
	if (_prop==p_bulletCollMarginScaling)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			App::ct->dynamicsContainer->setBulletDynamicCollisionMarginFactor(f,true);
	}


	// ODE parameters:
	if (_prop==p_odeTimeStep)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			App::ct->dynamicsContainer->setODEDynamicsStepSize(f*gv::userToSeconds,true);
	}
	if (_prop==p_odeQuickStep)
	{
		App::ct->dynamicsContainer->setODEUseQuickStep(((QtVariantProperty*)_prop)->value().toBool(),true);
		if (!App::ct->dynamicsContainer->getODEUseQuickStep())
			App::uiThread->messageBox_warning(this,strTranslate(IDS_ODE),strTranslate(IDS_WARNING_ODE_NOT_USING_QUICKSTEP),VMessageBox::OKELI);
	}
	if (_prop==p_odeIterations)
	{
		if (tt::stringToInt(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),a))
			App::ct->dynamicsContainer->setODEDynamicConstraintSolvingIterations(a,true);
	}
	if (_prop==p_odeInternalScaling)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			App::ct->dynamicsContainer->setODEDynamicInternalScalingFactor(f,true);
	}
	if (_prop==p_odeInternalFullScaling)
		App::ct->dynamicsContainer->setODEDynamicFullInternalScaling(((QtVariantProperty*)_prop)->value().toBool(),true);
	if (_prop==p_odeGlobalErp)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			App::ct->dynamicsContainer->setODEDynamicGlobalERP(f,true);
	}
	if (_prop==p_odeGlobalCfm)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			App::ct->dynamicsContainer->setODEDynamicGlobalCFM(f,true);
	}


	// Vortex parameters:
	std::vector<float> floatParams;
	std::vector<int> intParams;

	App::ct->dynamicsContainer->getVortexFloatParams(floatParams);
	App::ct->dynamicsContainer->getVortexIntParams(intParams);

	float stepSize=floatParams[0];
	float internalScalingFactor=floatParams[1];
	float contactTolerance=floatParams[2];
	float constraint_linear_compliance=floatParams[3];
	float constraint_linear_damping=floatParams[4];
	float constraint_linear_kineticLoss=floatParams[5];
	float constraint_angular_compliance=floatParams[6];
	float constraint_angular_damping=floatParams[7];
	float constraint_angular_kineticLoss=floatParams[8];
	// floatParams[9] IS RESERVED (was auto angular damping tension ratio)
	bool autoSleep=((intParams[0]&1)!=0);
	bool multiThreading=((intParams[0]&2)!=0);
	bool fullInternalScaling=((intParams[0]&4)!=0);
	// ((intParams[0]&8) IS RESERVED (was auto angular damping enabled flag)


	if (_prop==p_vortexTimeStep)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			stepSize=tt::userFloatToFloat(f,gv::userToSeconds,false);
	}
	if (_prop==p_vortexContactTolerance)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			contactTolerance=tt::userFloatToFloat(f,gv::userToMeter,false);
	}
	if (_prop==p_vortexInternalScaling)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			internalScalingFactor=f;
	}
	if (_prop==p_vortexInternalFullScaling)
		fullInternalScaling=((QtVariantProperty*)_prop)->value().toBool();
	if (_prop==p_vortexAutoSleep)
		autoSleep=((QtVariantProperty*)_prop)->value().toBool();
	if (_prop==p_vortexMultithreading)
		multiThreading=((QtVariantProperty*)_prop)->value().toBool();


	if (_prop==p_vortexConstraintLinearCompliance)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			constraint_linear_compliance=tt::userFloatToFloat(f,gv::userToSeconds*gv::userToSeconds/gv::userToKilograms,true);
	}
	if (_prop==p_vortexConstraintLinearDamping)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			constraint_linear_damping=tt::userFloatToFloat(f,gv::userToKilograms/gv::userToSeconds,true);
	}
	if (_prop==p_vortexConstraintLinearKinLoss)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			constraint_linear_kineticLoss=tt::userFloatToFloat(f,gv::userToSeconds/gv::userToKilograms,true);
	}
	if (_prop==p_vortexConstraintAngularCompliance)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			constraint_angular_compliance=tt::userFloatToFloat(f,gv::userToSeconds*gv::userToSeconds/(gv::userToKilograms*gv::userToMeter),true);
	}
	if (_prop==p_vortexConstraintAngularDamping)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			constraint_angular_damping=tt::userFloatToFloat(f,gv::userToKilograms*gv::userToMeter*gv::userToMeter/gv::userToSeconds,true);
	}
	if (_prop==p_vortexConstraintAngularKinLoss)
	{
		if (tt::stringToFloat(((QtVariantProperty*)_prop)->value().toString().toStdString().c_str(),f,false,false))
			constraint_angular_kineticLoss=tt::userFloatToFloat(f,gv::userToSeconds/(gv::userToKilograms*gv::userToMeter*gv::userToMeter),true);
	}


	floatParams[0]=stepSize;
	floatParams[1]=internalScalingFactor;
	floatParams[2]=contactTolerance;
	floatParams[3]=constraint_linear_compliance;
	floatParams[4]=constraint_linear_damping;
	floatParams[5]=constraint_linear_kineticLoss;
	floatParams[6]=constraint_angular_compliance;
	floatParams[7]=constraint_angular_damping;
	floatParams[8]=constraint_angular_kineticLoss;
	// floatParams[9] IS RESERVED (was auto angular damping tension ratio)

	intParams[0]=0;
	if (autoSleep)
		intParams[0]|=1;
	if (multiThreading)
		intParams[0]|=2;
	if (fullInternalScaling)
		intParams[0]|=4;
	// intParams[0]|=8 IS RESERVED (was auto angular damping enabled flag)

	App::ct->dynamicsContainer->setVortexFloatParams(floatParams,true);
	App::ct->dynamicsContainer->setVortexIntParams(intParams,true);
	refresh();
}
