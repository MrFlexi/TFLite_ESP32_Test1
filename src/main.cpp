/*=============================================================================
TensorFlow Lite Platformio Example

Author: Wezley Sherman
Referenced Authors: The TensorFlow Authors

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <Arduino.h>
#include <math.h>
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "sine_model_data.h"

int img_ankle[196] = {0,1,0,1,0,0,9,0,0,0,0,2,0,10,0,2,2,8,7,1,5,2,0,14,2,0,0,9,3,6,2,0,0,0,0,7,101,103,57,55,34,7,3,0,0,0,9,9,0,55,178,151,108,179,196,132,0,0,6,4,6,0,0,83,176,137,92,171,181,74,0,2,9,0,0,0,31,124,157,145,112,150,139,38,0,6,0,0,4,38,113,168,110,118,105,115,108,36,0,9,0,0,26,71,122,110,105,107,102,111,117,57,0,82,88,95,104,89,102,95,122,116,111,113,161,59,24,110,120,116,107,86,106,110,92,117,134,139,154,53,94,150,146,147,152,138,137,121,126,161,156,145,163,131,71,136,161,167,163,153,168,173,174,121,74,104,108,46,0,3,2,16,40,47,38,15,4,11,0,4,3,3,0,8,10,0,0,0,0,0,0,0,0,11,5,0};
int img_shirt[196] = {0,9,0,40,134,160,118,74,166,119,80,1,1,217,0,23,200,238,209,196,232,208,228,222,79,0,00,3,77,241,209,222,222,209,212,223,241,142,5,04,6,101,249,215,228,235,215,234,212,224,197,18,00,0,114,229,230,217,227,234,228,240,245,226,4,00,17,194,227,233,230,232,230,211,232,235,230,27,30,0,217,241,230,225,219,238,240,228,236,240,95,26,0,245,248,214,246,231,226,228,223,250,234,126,03,77,247,230,222,237,230,229,221,241,236,229,161,180,43,168,160,251,240,230,239,243,232,143,232,177,014,0,7,7,231,229,226,234,220,237,122,4,0,60,0,4,24,249,233,238,250,236,239,153,2,14,011,4,0,45,235,250,240,208,236,217,184,0,0,150,0,0,16,115,165,204,208,156,173,55,9,9,0};
int img_pullover[196] ={5,0,0,44,137,183,191,172,206,127,52,2,0,20,9,79,206,224,243,217,222,233,219,241,50,3,00,0,142,239,237,218,241,242,217,230,251,133,10,115,5,195,223,180,134,151,95,203,157,210,208,0,00,16,218,253,162,156,148,92,246,135,246,211,2,00,0,234,250,192,181,214,197,197,190,241,219,9,139,5,255,239,222,166,196,179,142,232,231,255,0,04,17,244,246,231,230,231,226,245,219,244,248,6,02,42,246,201,226,243,227,230,230,229,230,255,8,00,50,251,170,234,230,218,232,215,236,191,248,41,40,51,236,128,248,238,221,226,238,255,158,235,57,06,55,242,131,232,238,235,236,221,230,148,248,71,07,54,239,102,74,66,70,71,90,91,107,244,58,40,21,205,84,0,2,12,7,0,0,71,217,21,0};



// Create a memory pool for the nodes in the network
constexpr int tensor_pool_size = 6 * 1024;
uint8_t tensor_pool[tensor_pool_size];

char *labels[10] = { "T-Shirt", "Trowser", "Pullover", "Dress", "Coat", "Sandal", "Shirt", "Sneaker", "Bag", "Ankle boot"}; 
// Define the model to be used
const tflite::Model* sine_model;

// Define the interpreter
tflite::MicroInterpreter* interpreter;

// Input/Output nodes for the network
TfLiteTensor* input;
TfLiteTensor* output;


void readPic(float *data, int *image, int imageSize)
{
    
    int rxIdx = 0;

    for (int i = 0; i < imageSize; rxIdx++, i++)
            {
                data[rxIdx] = static_cast<float>(image[i]) / 255.0f;
				//Serial.println(data[rxIdx]);
            }
        
}


void predict_fashion(int *image)
{

// Set the input node to the user input
    	readPic( input->data.f, image, 196);

    	Serial.println("Running inference on inputted data...");

    	// Run inference on the input data
    	if(interpreter->Invoke() != kTfLiteOk) {
    		Serial.println("There was an error invoking the interpreter!");
    		return;
    	}

    	// Print the output of the model.
    	Serial.print("Fashion: ");

		
		
		char str[250] = {0};
    char buf[20] = {0};
	float val_old = 0;
	float val = 0;
	int max_index = 0;

    int numElements = output->dims->data[1];
	Serial.print("Elements: ");Serial.println(numElements);
	Serial.println("Probability per class: ");
    for (int i = 0; i < numElements; i++)
    {
        val = static_cast<float>(output->data.f[i]);
		sprintf(buf, "%.3f", val );
		//Serial.println(buf);
        strcat(str, buf);

		if ( val > val_old)
		{ 
			max_index = i;
			val_old = val;
		}
    }   

	Serial.println(str);
	Serial.println(labels[max_index]);

}


// Set up the ESP32's environment.
void setup() { 
	// Start serial at 115200 baud
	Serial.begin(115200);

	// Load the sample sine model
	Serial.println("Loading Tensorflow model....");
	//sine_model = tflite::GetModel(g_sine_model_data);
	sine_model = tflite::GetModel(fashion_mnist_model_tflite);

	Serial.println("Fashion MINST model loaded!");

	// Define ops resolver and error reporting
	static tflite::ops::micro::AllOpsResolver resolver;

	static tflite::ErrorReporter* error_reporter;
	static tflite::MicroErrorReporter micro_error;
	error_reporter = &micro_error;

	// Instantiate the interpreter 
	static tflite::MicroInterpreter static_interpreter(
		sine_model, resolver, tensor_pool, tensor_pool_size, error_reporter
	);

	interpreter = &static_interpreter;

	// Allocate the the model's tensors in the memory pool that was created.
	Serial.println("Allocating tensors to memory pool");
	if(interpreter->AllocateTensors() != kTfLiteOk) {
		Serial.println("There was an error allocating the memory...ooof");
		return;
	}

	// Define input and output nodes
	input = interpreter->input(0);
	output = interpreter->output(0);
	
	Serial.println();Serial.println();
	Serial.println("Ankle boots");
	predict_fashion(img_ankle);
	
	Serial.println();Serial.println();
	Serial.println("Pullover");
	predict_fashion(img_pullover);
	
	Serial.println();Serial.println();
	Serial.println("Shirt");
	predict_fashion(img_shirt);
}

// Logic loop for taking user input and outputting the sine
void loop() { 
	
} 