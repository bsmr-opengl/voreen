/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2008 Visualization and Computer Graphics Group, *
 * Department of Computer Science, University of Muenster, Germany.   *
 * <http://viscg.uni-muenster.de>                                     *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

#ifndef VRN_WIDGETGENERATORPLUGINS_H
#define VRN_WIDGETGENERATORPLUGINS_H

#include "widgetgenerator.h"

#include <QtGui>


namespace voreen {

/**
 * Widget representing the title bar of each processor shown in the autogenerated GUI.
 * The title bar contains the name of the processor and an +/- icon allowing to expand
 * the processors content.
 */	
class ProcessorHeaderWidget : public QWidget {
    Q_OBJECT
public:
    ProcessorHeaderWidget(QWidget* parent = 0, WidgetGenerator::ProcessorPropsList* actRenderer = 0, 
        MessageReceiver* msgReceiver=0, WidgetGenerator* widgetGen = 0, bool showCheckBox = true);

    void updateState();

public slots:
    void setVisRender();

private:
    WidgetGenerator* widgetGen_;
    MessageReceiver* msgReceiver_;
    WidgetGenerator::ProcessorPropsList* actRenderer_;

	QPushButton* expandButton_;
};


class OverlayProcessorHeaderWidget : public QWidget{
    Q_OBJECT
public:
    OverlayProcessorHeaderWidget(QWidget* parent = 0, WidgetGenerator::OverlayPropsList* actOverlayPair = 0, MessageReceiver* msgReceiver=0, WidgetGenerator* widgetGen = 0, bool showCheckBox = true);

public slots:
    void setVisOverlay();

private:
    WidgetGenerator* widgetGen_;
    MessageReceiver* msgReceiver_;
    WidgetGenerator::OverlayPropsList* actOverlay_;

};

/* -------------------------------------------------------------------------------------------- */

class AGFloatWidget : public QWidget,public TemplatePlugin<float>{
    Q_OBJECT
public:
    /**
     *Creates a new AGFloatWidget - adds slider and spinbox to given parent and create slot for it
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGFloatWidget(QWidget* parent = 0, MessageReceiver* msgReceiver = 0, FloatProp* prop = 0);

    void setVisibleState(bool vis);
    void setFrameControler(QGroupBox* box);
    void sendMessage();
    void setRange();
    void initWidgetFromLoadedSettings(float min, float max, float value);

    /**
     *Callback-method of AGFloatWidget - sets the value of slider and spinbox to f
     *
     *@param f value
     */
    virtual void changeValue(float f);

    
public slots:
    /**
     *SLOT-method of slider 
     *posts message when slidervalue was changed
     */
    void sliderChanged(int value);
    /**
     *SLOT-method of spinbox
     *posts message when spinboxvalue was changed
     */
    void spinboxChanged(double value);
    /**
     *SLOT-method of slider - posts repaint-message if tracking is disabled
     */
    void released();
    /**
     *SLOT-method of slider - calls sliderChanged
     */
    void sliderDragged(int value);
private:
    FloatProp* myProp_;
    MessageReceiver* msgReceiver_;
    Identifier msgIdent_;
    QSlider* slider_;
    QDoubleSpinBox* spinBox_;
    QWidget* parWidget_;
    int sliderValue_;
    double spinValue_;
    float minimum_;
    float maximum_;
    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler
};

/* -------------------------------------------------------------------------------------------- */

class AGIntWidget : public QWidget,public TemplatePlugin<int>{
    Q_OBJECT
public:
    /**
     *Creates a new AGIntWidget - adds slider and spinbox to given parent and create slot for it
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGIntWidget(QWidget* parent = 0, MessageReceiver* msgreceiver = 0, IntProp* prop = 0);

    void setVisibleState(bool vis);
    void setFrameControler(QGroupBox* box);
    void sendMessage();
    void initWidgetFromLoadedSettings(int min, int max, int value);

    /**
     *Callback-method of AGIntWidget - sets the value of slider and spinbox to i
     *
     *@param i value
     */
    virtual void changeValue(int i);

    /**
     *Sets the ranges of the slider and spinbox
     *
     *@param min minimum value of slider and spinbox
     *@param max maximum value of slider and spinbox
     */
    void setRange(int min, int max);
public slots:
    /**
     *SLOT-method of slider - posts message when slidervalue or spinboxvalue was changed
     */
    void valChanged(int value);
    /**
     *SLOT-method of slider - posts repaint-message if tracking is disabled
     */
    void released();
    /**
     *SLOT-method of slider - calls valChanged
     */
    void sliderDragged(int value);
private:
    IntProp* myProp_;
    MessageReceiver* msgReceiver_;
    Identifier msgIdent_;
    QSlider* slider_;
    QSpinBox* spinBox_;
    QWidget* parWidget_;
    int value_;
    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler
};

/* -------------------------------------------------------------------------------------------- */

class AGBoolWidget : public QWidget,public TemplatePlugin<bool>{
    Q_OBJECT
public:
    /**
     *Creates a new AGBoolWidget - adds checkbox to given parent and create slot for it
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGBoolWidget(QWidget* parent = 0, MessageReceiver* msgreceiver = 0, BoolProp* prop = 0);

    void setVisibleState(bool vis);
    void setFrameControler(QGroupBox* box);
    void initWidgetFromLoadedSettings(bool value);

    /**
     *Callback-method of AGBoolWidget - sets the status of checkbox to given value
     *
     *@param b bool
     */
    virtual void changeValue(bool b);
public slots:
    /**
     *SLOT-method of checkbox - posts message when status was changed
     */
    void valChanged(bool value);
private:
    BoolProp* myProp_;
    MessageReceiver* msgReceiver_;
    QCheckBox* chBox_;
    Identifier msgIdent_;
    QWidget* parWidget_;
    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler

};

/* -------------------------------------------------------------------------------------------- */

class AGColorWidget : public QWidget,public TemplatePlugin<tgt::vec4>{
    Q_OBJECT
public:
    /**
     *Creates a new AGColorWidget - adds button to given parent and create slot for it
     *additionally a palette is added to display the current active color
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGColorWidget(QWidget* parent = 0, MessageReceiver* msgreceiver = 0, ColorProp* prop = 0);

    void setVisibleState(bool vis);
    void setFrameControler(QGroupBox* box);
    void sendMessage();
    void initWidgetFromLoadedSettings(tgt::Color value);

    /**
     *Callback-method of AGColorWidget - sets the current active color
     *
     *@param col color
     */
    virtual void changeValue(tgt::Color col);
public slots:
    /**
     *SLOT-method of pushbutton - opens colordialog and posts a color-message
     */
    void clicked(void);
private:
    ColorProp* myProp_;
    MessageReceiver* msgReceiver_;	
    QPushButton* btnCol_;
    QLabel* myLabel_;
    Identifier msgIdent_;
    QWidget* parWidget_;
    tgt::Color curColor_;
    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler
};

/* -------------------------------------------------------------------------------------------- */

class AGEnumWidget : public QWidget,public TemplatePlugin<int>{
    Q_OBJECT
public:
    /**
     *Creates a new AGEnumWidget - add combobox to given parent and create slot for it
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGEnumWidget(QWidget* parent = 0, MessageReceiver* msgreceiver = 0, EnumProp* prop = 0);

    void setVisibleState(bool vis);
    void setFrameControler(QGroupBox* box);
    void initWidgetFromLoadedSettings(int index);

    /**
     *Sets the strings that will appear in the combobox
     *
     *@param str Vector of strings that will be the names of items in the combobox
     */
    void setStrings(const std::vector<std::string>& str);

    /**
     *Callback-method of AGEnumWidget - sets the startindex and items in the combobox
     *to given values
     *
     *@param index index to what item the combobox will be set
     */
    virtual void changeValue(int index);
    /**
     * Retrieves property values and sets widget to those values.
     * Signals are blocked
     */
    virtual void updateFromProperty();

public slots:
    /**
     *SLOT-method of combobox - posts String-message
     *
     *@param value index of the item that was selected
     */
    void valChanged(int val);

private:
    EnumProp* myProp_;
    MessageReceiver* msgReceiver_;
    QComboBox* box_;
    std::vector<std::string> strings_;
    bool sendStringMsg_;
    Identifier msgIdent_;
    QWidget* parWidget_;

    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler
};

/* -------------------------------------------------------------------------------------------- */

class AGButtonWidget : public QWidget,public TemplatePlugin< bool >{
    Q_OBJECT
public:
    /**
     *Creates a new AGButtonWidget - the button send a (dummy) bool msg
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGButtonWidget(QWidget* parent = 0, MessageReceiver* msgreceiver = 0, ButtonProp* prop = 0);

    void setVisibleState(bool vis);
    void setFrameControler(QGroupBox* box);

    /**
     *Dummy Callback-method of AGButtonWidget - callback for button makes no sense
     */
    virtual void changeValue();

public slots:
    /**
     *SLOT-method of pushButton - sends the identifier that will be used in postMessage when the button is clicked
     */
    void clicked();
private:
    ButtonProp* myProp_;
    MessageReceiver* msgReceiver_;	
    QPushButton* btnButton_;
    Identifier msgIdent_;
    QWidget* parWidget_;
    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler
};

/* -------------------------------------------------------------------------------------------- */

class AGFileDialogWidget : public QWidget,public TemplatePlugin< std::string >{
    Q_OBJECT
public:
    /**
     *Creates a new AGButtonWidget - the button send a (dummy) bool msg
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGFileDialogWidget(QWidget* parent = 0, MessageReceiver* msgreceiver = 0, FileDialogProp* prop = 0);

    void setVisibleState(bool vis);
    void setFrameControler(QGroupBox* box);
    /**
     *Dummy Callback-method of AGButtonWidget - callback for button makes no sense
     */
    virtual void changeValue();

public slots:
    /**
     *SLOT-method of pushbutton - opens colordialog and posts a color-message
     */
    void clicked(void);

private:
    FileDialogProp* myProp_;
    MessageReceiver* msgReceiver_;	
    QPushButton* openFileDialogBtn_;
    Identifier msgIdent_;
    QWidget* parWidget_;

    std::string dialogCaption_;
    std::string directory_;
    std::string fileFilter_;
    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler
};

/* -------------------------------------------------------------------------------------------- */

class AGIntVec2Widget : public QWidget,public TemplatePlugin<tgt::ivec2>{
    Q_OBJECT
public:
    /**
     *Creates a new intVec2Plugin - adds slider and spinbox to given parent and create slot for it
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGIntVec2Widget(QWidget* parent = 0, MessageReceiver* msgreceiver = 0, IntVec2Prop* prop = 0);

    /**
     *Callback-method of intVec2Plugin - sets the value of spinboxes to vec
     *
     *@param vec value
     */
	virtual void changeValue(tgt::ivec2 vec);

    /**
     *Sets the ranges of the spinboxes
     *
     *@param min minimum value of spinboxes
     *@param max maximum value of spinboxes
     */
	void setRange(tgt::ivec2 min, tgt::ivec2 max);

    void setFrameControler(QGroupBox* box);
    void setVisibleState(bool vis);
    void sendMessage();

    void initWidgetFromLoadedSettings(tgt::ivec2 min, tgt::ivec2 max, tgt::ivec2 value);

public slots:
    /**
     *SLOT-method of x-spinbox 
     */
    void valChangedX(int value);
	/**
     *SLOT-method of y-spinbox 
     */
    void valChangedY(int value);
private:
    IntVec2Prop* myProp_;
    MessageReceiver* msgReceiver_;
    Identifier msgIdent_;
    QSpinBox* spinBox_x;
	QSpinBox* spinBox_y;
    QWidget* parWidget_;
	tgt::ivec2 value_;
    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler
};

/* -------------------------------------------------------------------------------------------- */

class AGIntVec3Widget : public QWidget,public TemplatePlugin<tgt::ivec3>{
    Q_OBJECT
public:
    /**
     *Creates a new intVec3Plugin - adds slider and spinbox to given parent and create slot for it
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGIntVec3Widget(QWidget* parent = 0, MessageReceiver* msgreceiver = 0, IntVec3Prop* prop = 0);

    /**
     *Callback-method of intVec3Plugin - sets the value of spinboxes to vec
     *
     *@param vec value
     */
	virtual void changeValue(tgt::ivec3 vec);

    /**
     *Sets the ranges of the spinboxes
     *
     *@param min minimum value of spinboxes
     *@param max maximum value of spinboxes
     */
	void setRange(tgt::ivec3 min, tgt::ivec3 max);

    void setFrameControler(QGroupBox* box);
    void setVisibleState(bool vis);
  	void sendMessage();
    void initWidgetFromLoadedSettings(tgt::ivec3 min, tgt::ivec3 max, tgt::ivec3 value);

public slots:
    /**
     *SLOT-method of spinboxes
     */
    void valChangedX(int value);
    /**
     *SLOT-method of spinboxes
     */
    void valChangedY(int value);
    /**
     *SLOT-method of spinboxes
     */
    void valChangedZ(int value);
private:
	IntVec3Prop* prop_;
    MessageReceiver* msgReceiver_;
    Identifier msgIdent_;
    QSpinBox* spinBox_x;
	QSpinBox* spinBox_y;
	QSpinBox* spinBox_z;
    QWidget* parWidget_;
	tgt::ivec3 value_;
    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler
};

/* -------------------------------------------------------------------------------------------- */

class AGIntVec4Widget : public QWidget,public TemplatePlugin<tgt::ivec4>{
    Q_OBJECT
public:
    /**
     *Creates a new intVec4Plugin - adds slider and spinbox to given parent and create slot for it
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGIntVec4Widget(QWidget* parent = 0, MessageReceiver* msgreceiver = 0, IntVec4Prop* prop = 0);

    /**
     *Callback-method of intVec4Plugin - sets the value of spinboxes to vec
     *
     *@param vec value
     */
	virtual void changeValue(tgt::ivec4 vec);

    /**
     *Sets the ranges of the spinboxes
     *
     *@param min minimum value of spinboxes
     *@param max maximum value of spinboxes
     */
	void setRange(tgt::ivec4 min, tgt::ivec4 max);

    void setFrameControler(QGroupBox* box);
    void setVisibleState(bool vis);
	void sendMessage();
    void initWidgetFromLoadedSettings(tgt::ivec4 min, tgt::ivec4 max, tgt::ivec4 value);
    
public slots:
    /**
     *SLOT-method of spinbox x
     */
    void valChangedX(int value);
    /**
     *SLOT-method of spinbox y
     */
    void valChangedY(int value);
    /**
     *SLOT-method of spinbox z
     */
    void valChangedZ(int value);
	/**
     *SLOT-method of spinbox w
     */
    void valChangedW(int value);
private:
	IntVec4Prop* prop_;
    MessageReceiver* msgReceiver_;
    Identifier msgIdent_;
    QSpinBox* spinBox_x;
	QSpinBox* spinBox_y;
	QSpinBox* spinBox_z;
	QSpinBox* spinBox_w;
    QWidget* parWidget_;
	tgt::ivec4 value_;
    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler
};
/* -------------------------------------------------------------------------------------------- */

class AGFloatVec2Widget : public QWidget,public TemplatePlugin<tgt::vec2>{
    Q_OBJECT
public:
    /**
     *Creates a new AGFloatVec2Widget - adds spinbox to given parent and create slot for it
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGFloatVec2Widget(QWidget* parent = 0, MessageReceiver* msgreceiver = 0, FloatVec2Prop* prop = 0);

    /**
     *Callback-method of AGFloatVec2Widget - sets the value of spinboxes to vec
     *
     *@param vec value
     */
	virtual void changeValue(tgt::vec2 vec);

    /**
     *Sets the ranges of the spinboxes
     *
     *@param min minimum value of spinboxes
     *@param max maximum value of spinboxes
     */
	void setRange(tgt::vec2 min, tgt::vec2 max);

    void setFrameControler(QGroupBox* box);
    void setVisibleState(bool vis);
  	void sendMessage();
    void initWidgetFromLoadedSettings(tgt::vec2 min, tgt::vec2 max, tgt::vec2 value);

public slots:
    /**
     *SLOT-method of spinbox x
     */
    void valChangedX(double value);
    /**
     *SLOT-method of spinbox y
     */
    void valChangedY(double value);
private:
	int calculateDecimals(float min, float max);

	FloatVec2Prop* prop_;
    MessageReceiver* msgReceiver_;
    Identifier msgIdent_;
    QDoubleSpinBox* spinBox_x;
	QDoubleSpinBox* spinBox_y;
    QWidget* parWidget_;
	tgt::vec2 value_;
    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler
};

/* -------------------------------------------------------------------------------------------- */

class AGFloatVec3Widget : public QWidget,public TemplatePlugin<tgt::vec3>{
    Q_OBJECT
public:
    /**
     *Creates a new AGFloatVec3Widget - adds spinbox to given parent and create slot for it
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGFloatVec3Widget(QWidget* parent = 0, MessageReceiver* msgreceiver = 0, FloatVec3Prop* prop = 0);

    /**
     *Callback-method of AGFloatVec3Widget - sets the value of spinboxes to vec
     *
     *@param vec value
     */
	virtual void changeValue(tgt::vec3 vec);

    /**
     *Sets the ranges of the spinboxes
     *
     *@param min minimum value of spinboxes
     *@param max maximum value of spinboxes
     */
	void setRange(tgt::vec3 min, tgt::vec3 max);

    void setFrameControler(QGroupBox* box);
    void setVisibleState(bool vis);
  	void sendMessage();
    void initWidgetFromLoadedSettings(tgt::vec3 min, tgt::vec3 max, tgt::vec3 value);

public slots:
    /**
     *SLOT-method of spinbox x
     */
    void valChangedX(double value);
    /**
     *SLOT-method of spinbox y
     */
    void valChangedY(double value);
    /**
     *SLOT-method of spinbox z
     */
    void valChangedZ(double value);
private:
	int calculateDecimals(float min, float max);

	FloatVec3Prop* prop_;
    MessageReceiver* msgReceiver_;
    Identifier msgIdent_;
    QDoubleSpinBox* spinBox_x;
	QDoubleSpinBox* spinBox_y;
	QDoubleSpinBox* spinBox_z;
    QWidget* parWidget_;
	tgt::vec3 value_;
    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler
};

/* -------------------------------------------------------------------------------------------- */

class AGFloatVec4Widget : public QWidget,public TemplatePlugin<tgt::vec4>{
    Q_OBJECT
public:
    /**
     *Creates a new AGFloatVec4Widget - adds spinbox to given parent and create slot for it
     *
     *@param parent parent object
     *@param msgReceiver msgReceiver
     */
	AGFloatVec4Widget(QWidget* parent = 0, MessageReceiver* msgreceiver = 0, FloatVec4Prop* prop = 0);

    /**
     *Callback-method of AGFloatVec4Widget - sets the value of spinboxes to vec
     *
     *@param vec value
     */
	virtual void changeValue(tgt::vec4 vec);

    /**
     *Sets the ranges of the spinboxes
     *
     *@param min minimum value of spinboxes
     *@param max maximum value of spinboxes
     */
	void setRange(tgt::vec4 min, tgt::vec4 max);

    void setFrameControler(QGroupBox* box);
    void setVisibleState(bool vis);
	void sendMessage();
    void initWidgetFromLoadedSettings(tgt::vec4 min, tgt::vec4 max, tgt::vec4 value);

public slots:
    /**
     *SLOT-method of spinbox x
     */
    void valChangedX(double value);
    /**
     *SLOT-method of spinbox y
     */
    void valChangedY(double value);
    /**
     *SLOT-method of spinbox z
     */
    void valChangedZ(double value);
	/**
     *SLOT-method of spinbox w
     */
    void valChangedW(double value);
private:
	int calculateDecimals(float min, float max);

	FloatVec4Prop* prop_;
    MessageReceiver* msgReceiver_;
    Identifier msgIdent_;
    QDoubleSpinBox* spinBox_x;
	QDoubleSpinBox* spinBox_y;
	QDoubleSpinBox* spinBox_z;
	QDoubleSpinBox* spinBox_w;
    QWidget* parWidget_;
	tgt::vec4 value_;
    QGroupBox* groupBox_; // equals zero if Plugin is not FrameControler
};

} // namespace voreen

#endif //VRN_WIDGETGENERATORPLUGINS_H
