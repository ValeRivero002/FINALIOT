const Alexa = require('ask-sdk-core');
const AWS = require('aws-sdk');
const IotData = new AWS.IotData({endpoint: 'a2dbaocrje4klj-ats.iot.us-east-2.amazonaws.com'});
const dynamoDB = new AWS.DynamoDB.DocumentClient();
const TABLE_NAME = 'smart_dispenser_users_data';

async function getUserThings(userId) {
    const params = {
        TableName: TABLE_NAME,
        Key: {
            user_id: userId
        }
    };
    try {
        const data = await dynamoDB.get(params).promise();
        return data.Item;
    } catch (err) {
        console.error("Error al obtener datos de DynamoDB:", err);
        return null;
    }
}

let TurnOffParams = {
  thingName: '',
  payload: '{"state": {"desired": {"dispenser_state": "closed"}}}',
};

let TurnOnParams = {
  thingName: '',
  payload: '{"state": {"desired": {"dispenser_state": "open"}}}',
};

let ShadowParams = {
  thingName: '',
};

function getShadowPromise(params) {
    return new Promise((resolve, reject) => {
        IotData.getThingShadow(params, (err, data) => {
            if (err) {
                console.log(err, err.stack);
                reject('Failed to get thing shadow ${err.errorMessage}');
            } else {
                resolve(JSON.parse(data.payload));
            }
        });
    });
}

const LaunchRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'LaunchRequest';
    },

    
    async handle(handlerInput) {
        const userId = handlerInput.requestEnvelope.session.user.userId;
        const userData = await getUserThings(userId);
        datauser = userData;
        if (userData && userData.things && Array.isArray(userData.things)) {
            const filteredThings = userData.things.filter(obj => obj.includes("smart_dispenser"))
            TurnOffParams.thingName = filteredThings[0];
            TurnOnParams.thingName = filteredThings[0];
            ShadowParams.thingName = filteredThings[0];
            
            const speakOutput = 'Bienvenido al dispersador de comida, puede saber si le esta dando comida, cuanta comida tiene, dar comida y detenerla, que quieres hacer?';
            
            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        }else{       
            const speakOutput = 'Lo siento, no estás autorizado para usar esta habilidad.';
            return handlerInput.responseBuilder
                .speak(speakOutput)
                .withShouldEndSession(true)
                .getResponse();
        }
    }
};

const StateIntentDispenserHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'StateIntentDispenser';
    },
    async handle(handlerInput) {

        var builtInLed = 'unknown';
        await getShadowPromise(ShadowParams).then((result) => dispenser_state = result.state.reported.dispenser_state);
        console.log(dispenser_state);

        var speakOutput = 'Error';
        if (dispenser_state == "open") {
            speakOutput = 'Se esta dando comida';
        } else if (dispenser_state == "closed") {
            speakOutput = 'No se esta dando comida';
        } else {
            speakOutput = 'No se pudo consultar si se esta dando o no comida, por favor intente más tarde';
        }   

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const StateIntentWeightHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'StateIntentWeight';
    },
    async handle(handlerInput) {

        var builtInLed = 'unknown';
        await getShadowPromise(ShadowParams).then((result) => weight_state = result.state.reported.weight);
        console.log(weight_state);

        var speakOutput = 'Error';
        if (weight_state  == "nothing") {
            speakOutput = 'No tiene nada de comida';
        } else if (weight_state  == "little") {
            speakOutput = 'Tiene poca comida';
        } else if (weight_state  == "enough") {
            speakOutput = 'Tiene suficiente comida';
        } else if (weight_state  == "overflowing") {
            speakOutput = 'Tiene un exceso de comida, tenga precaución y vea si no hay nada interfiriendo con el plato';
        } else {
            speakOutput = 'No se pudo consultar cuanta comida tiene, por favor intente más tarde';
        }     

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const TurnOffIntentHandler = {
  canHandle(handlerInput) {
      return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
          && Alexa.getIntentName(handlerInput.requestEnvelope) === 'TurnOffIntent';
  },
  handle(handlerInput) {
    var speakOutput = "Error";
    IotData.updateThingShadow(TurnOffParams, function(err, data) {
        if (err) console.log(err);
    });
  
    speakOutput = 'Solicitaste dejar de dar comida';

      return handlerInput.responseBuilder
          .speak(speakOutput)
          .reprompt(speakOutput)
          .getResponse();
  }
};

const TurnOnIntentHandler = {
  canHandle(handlerInput) {
      return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
          && Alexa.getIntentName(handlerInput.requestEnvelope) === 'TurnOnIntent';
  },
  handle(handlerInput) {
    var speakOutput = "Error";
    IotData.updateThingShadow(TurnOnParams, function(err, data) {
        if (err) console.log(err);
    });
  
    speakOutput = 'Solicitaste dar comida';

      return handlerInput.responseBuilder
          .speak(speakOutput)
          .reprompt(speakOutput)
          .getResponse();
  }
};

const HelpIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.HelpIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'tienes 4 funciones: dar comida, dejar de dar comida, saber si le esta dando comida y el peso de la comida';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CancelAndStopIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && (Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.CancelIntent'
                || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.StopIntent');
    },
    handle(handlerInput) {
        const speakOutput = 'Adios';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .getResponse();
    }
};
/* *
 * FallbackIntent triggers when a customer says something that doesn’t map to any intents in your skill
 * It must also be defined in the language model (if the locale supports it)
 * This handler can be safely added but will be ingnored in locales that do not support it yet 
 * */
const FallbackIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.FallbackIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Sorry, I don\'t know about that. Please try again.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};
/* *
 * SessionEndedRequest notifies that a session was ended. This handler will be triggered when a currently open 
 * session is closed for one of the following reasons: 1) The user says "exit" or "quit". 2) The user does not 
 * respond or says something that does not match an intent defined in your voice model. 3) An error occurs 
 * */
const SessionEndedRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'SessionEndedRequest';
    },
    handle(handlerInput) {
        console.log(`~~~~ Session ended: ${JSON.stringify(handlerInput.requestEnvelope)}`);
        // Any cleanup logic goes here.
        return handlerInput.responseBuilder.getResponse(); // notice we send an empty response
    }
};
/* *
 * The intent reflector is used for interaction model testing and debugging.
 * It will simply repeat the intent the user said. You can create custom handlers for your intents 
 * by defining them above, then also adding them to the request handler chain below 
 * */
const IntentReflectorHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest';
    },
    handle(handlerInput) {
        const intentName = Alexa.getIntentName(handlerInput.requestEnvelope);
        const speakOutput = `You just triggered ${intentName}`;

        return handlerInput.responseBuilder
            .speak(speakOutput)
            //.reprompt('add a reprompt if you want to keep the session open for the user to respond')
            .getResponse();
    }
};
/**
 * Generic error handling to capture any syntax or routing errors. If you receive an error
 * stating the request handler chain is not found, you have not implemented a handler for
 * the intent being invoked or included it in the skill builder below 
 * */
const ErrorHandler = {
    canHandle() {
        return true;
    },
    handle(handlerInput, error) {
        const speakOutput = 'Sorry, I had trouble doing what you asked. Please try again.';
        console.log(`~~~~ Error handled: ${JSON.stringify(error)}`);

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

/**
 * This handler acts as the entry point for your skill, routing all request and response
 * payloads to the handlers above. Make sure any new handlers or interceptors you've
 * defined are included below. The order matters - they're processed top to bottom 
 * */
exports.handler = Alexa.SkillBuilders.custom()
    .addRequestHandlers(
        LaunchRequestHandler,
        StateIntentDispenserHandler,
        StateIntentWeightHandler,
        TurnOnIntentHandler,
        TurnOffIntentHandler,
        HelpIntentHandler,
        CancelAndStopIntentHandler,
        FallbackIntentHandler,
        SessionEndedRequestHandler,
        IntentReflectorHandler)
    .addErrorHandlers(
        ErrorHandler)
    .withCustomUserAgent('sample/hello-world/v1.2')
    .lambda();
