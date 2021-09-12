#include "plugin.hpp"

// Be warned that, as the immortal programmar Roast Beef said, "My code is a Dog's code."
// Gratitude to Mannequins for letting me create this humble clone of their awesome work as a way to learn VCVrack :D

// todo: create an expander which adds dedicated macro knob output, envelope follower rise/fall settings, location speed knob, etc etc

// Introduce simple filter class which blocks DC signal (for mixer)
class DCBlocker
{
public:
    DCBlocker() {}
    float process(float input)
    {
        float y = 0.005 * xm1 + 0.995 * ym1;
        xm1 = input;
        ym1 = y;
        return input - y;
        /* ALTERNATE METHOD
        float y = input - xm1 + 0.995 * ym1;
        xm1 = input;
        ym1 = y;
        return y;
        */
    }
private:
    float xm1 = 0.0f;
    float ym1 = 0.0f;
};



struct Chilly_cheese : Module {
  // context menu stuff so you can switch between different modes and suchlike
  int mode = 0;

	void onReset() override {
		mode = 0;
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "mode", json_integer(mode));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* modeJ = json_object_get(rootJ, "mode");
		if (modeJ)
			mode = json_integer_value(modeJ);
	}

  // initialize location, slew, dc blockers
	float location = 0.f;
	dsp::RCFilter slew;
  DCBlocker dc1, dc2, dc3, dc4, dc5, dc6;
	enum ParamIds {
		MACRO_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		MACRO_INPUT,
		FADE_INPUT,
		LEFT_INPUT,
		OFFSET_INPUT,
		RIGHT_INPUT,
		OR2_INPUT,
		OR1_INPUT,
		AND2_INPUT,
		AND1_INPUT,
		SLOPE_INPUT,
		CREASE_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		OR_OUTPUT,
		AND_OUTPUT,
		SLOPE_OUTPUT,
		FOLLOW_OUTPUT,
		CREASE_OUTPUT,
		LOCATION_OUTPUT,
		CHEESE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Chilly_cheese() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(MACRO_PARAM, 0.f, 1.f, 0.5f, "MACRO");
	}

	void process(const ProcessArgs& args) override {
		// setup macro and macro cv
		float macro = params[MACRO_PARAM].getValue();
		float macrovolts = (macro * 10) - 5;
		float macro_cvin = 0.f;
		if ( inputs[MACRO_INPUT].isConnected() ) {
			macro_cvin = inputs[MACRO_INPUT].getVoltage();
		}
		macro_cvin = clamp(macro_cvin, -5.f, 5.f);
		macro += (macro_cvin / 10);
		macro = clamp(macro, 0.f, 1.f);
		macrovolts += macro_cvin;
		macrovolts = clamp(macrovolts, -5.f, 5.f);
		// initial jack normalling
		float left = -5.f;
		float right = 5.f;
		float fade = macro;
		float or2, and2, slope;
		or2 = and2 = slope = macrovolts;
		// left and right crossfader calc and normalling
		if ( inputs[LEFT_INPUT].isConnected() ) {
			left = inputs[LEFT_INPUT].getVoltage();
		}
		if ( inputs[RIGHT_INPUT].isConnected() ) {
			right = inputs[RIGHT_INPUT].getVoltage();
		}
		if ( inputs[FADE_INPUT].isConnected() ) {
			fade = inputs[FADE_INPUT].getVoltage();
			fade = (fade / 10.0f) + 0.5f;
			fade = clamp(fade, 0.f, 1.f);
		}
		float offset = inputs[OFFSET_INPUT].getVoltage();
		float leftfaded = ( ( 1.f - fade) * left ) + ( fade * right ) + offset;
		float rightfaded = ( ( 1.f - fade) * right ) + ( fade * left ) + offset;
		leftfaded = clamp(leftfaded, -5.f, 5.f);
		rightfaded = clamp(rightfaded, -5.f, 5.f);
		outputs[LEFT_OUTPUT].setVoltage(leftfaded);
		outputs[RIGHT_OUTPUT].setVoltage(rightfaded);
		// begin the cheese mixture
		float cheese = dc1.process(left) + dc2.process(right);
		// or + and calc and normalling
		float or1, or_output, and1, and_output;
		or1 = and1 = inputs[OR1_INPUT].getVoltage();
		if ( inputs[OR2_INPUT].isConnected() ) {
			or2 = and2 = inputs[OR2_INPUT].getVoltage();
		}
		if (or1 >= or2) {
			or_output = or1;
		} else {
			or_output = or2;
		}
		or_output = clamp(or_output, -5.f, 5.f);
		outputs[OR_OUTPUT].setVoltage(or_output);
		if ( inputs[AND1_INPUT].isConnected() ) {
			and1 = inputs[AND1_INPUT].getVoltage();
		}
		if ( inputs[AND2_INPUT].isConnected() ) {
			and2 = inputs[AND2_INPUT].getVoltage();
		}
		if (and1 <= and2) {
			and_output = and1;
		} else {
			and_output = and2;
		}
		and_output = clamp(and_output, -5.f, 5.f);
		outputs[AND_OUTPUT].setVoltage(and_output);
		// continue the cheese mixture
		cheese = cheese + dc3.process(or1) + dc4.process(and1);
		// slope and follow calc
		if ( inputs[SLOPE_INPUT].isConnected() ) {
			slope = inputs[SLOPE_INPUT].getVoltage();
		}
		cheese += dc5.process(slope); // penultimate cheese ingredient added
		float crease = slope;
		slope = clamp(slope, -5.f, 5.f);
		slope = abs(slope);
    slew.setCutoffFreq(20.0f / args.sampleRate); // make this configurable in the future (context menu / expander?)
		slew.process(slope);
    float follow = slew.lowpass();
    follow = sqrt(follow*5)*2.5;
    follow = clamp(follow, -5.f, 5.f);
		outputs[SLOPE_OUTPUT].setVoltage(slope);
		outputs[FOLLOW_OUTPUT].setVoltage(follow);
		// crease/location calc and normalling
		if ( inputs[CREASE_INPUT].isConnected() ) {
			crease = inputs[CREASE_INPUT].getVoltage();
		}
    // "location" aka Integrator has 6 different modes - default, glacial, sluggish, slowish, quickish, and snappy
    switch(mode) {
      case 0 :
        location += (crease/25000);
        break;
      case 1 :
        location += (crease/300000);
        break;
      case 2 :
        location += (crease/100000);
        break;
      case 3 :
        location += (crease/50000);
        break;
      case 4 :
        location += (crease/12500);
        break;
      case 5 :
        location += (crease/6250);
        break;
    }
    location = clamp(location, -5.f, 5.f);
    outputs[LOCATION_OUTPUT].setVoltage(location);
		cheese += dc6.process(crease); // final cheese ingredient added
		if ( crease >= 0.f ) {
			crease -= 5.f;
		} else {
			crease += 5.f;
		}
		crease = clamp(crease, -5.f, 5.f);
		outputs[CREASE_OUTPUT].setVoltage(crease);
		// PREPARE THE CHEESE!!!
		cheese = clamp(cheese*macro, -10.0f, 10.0f);
		outputs[CHEESE_OUTPUT].setVoltage(cheese);
	}
};


struct Chilly_cheeseWidget : ModuleWidget {
	Chilly_cheeseWidget(Chilly_cheese* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/chilly_cheese.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(Vec(20.026, 20.232)), module, Chilly_cheese::MACRO_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.863, 38.04)), module, Chilly_cheese::MACRO_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.73, 44.832)), module, Chilly_cheese::FADE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.688, 51.383)), module, Chilly_cheese::LEFT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.622, 58.037)), module, Chilly_cheese::OFFSET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.723, 64.571)), module, Chilly_cheese::RIGHT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.664, 71.15)), module, Chilly_cheese::OR2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.555, 77.472)), module, Chilly_cheese::OR1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.58, 84.346)), module, Chilly_cheese::AND2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.642, 90.446)), module, Chilly_cheese::AND1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.304, 103.82)), module, Chilly_cheese::SLOPE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.22, 117.04)), module, Chilly_cheese::CREASE_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.877, 38.213)), module, Chilly_cheese::LEFT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.056, 51.271)), module, Chilly_cheese::RIGHT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.024, 64.772)), module, Chilly_cheese::OR_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.997, 77.686)), module, Chilly_cheese::AND_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.056, 90.685)), module, Chilly_cheese::SLOPE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.369, 97.079)), module, Chilly_cheese::FOLLOW_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.056, 103.504)), module, Chilly_cheese::CREASE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.538, 110.361)), module, Chilly_cheese::LOCATION_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.997, 116.503)), module, Chilly_cheese::CHEESE_OUTPUT));
	}

  void appendContextMenu(Menu* menu) override {
    Chilly_cheese* module = dynamic_cast<Chilly_cheese*>(this->module);

    menu->addChild(new MenuEntry);
    menu->addChild(createMenuLabel("Mode"));

    struct ModeItem : MenuItem {
      Chilly_cheese* module;
      int mode;
      void onAction(const event::Action& e) override {
        module->mode = mode;
      }
    };

    std::string modeNames[6] = {"Default Location Speed", "Glacial Location Speed", "Sluggish Location Speed", "Slowish Location Speed", "Quickish Location Speed", "Snappy Location Speed"};
    for (int i = 0; i < 6; i++) {
      ModeItem* modeItem = createMenuItem<ModeItem>(modeNames[i]);
      modeItem->rightText = CHECKMARK(module->mode == i);
      modeItem->module = module;
      modeItem->mode = i;
      menu->addChild(modeItem);
    }
  }
};


Model* modelChilly_cheese = createModel<Chilly_cheese, Chilly_cheeseWidget>("chilly_cheese");
