import json
import boto3

client = boto3.client('iot-data')

def lambda_handler(event, context):
    try:
        thing_name = event.get('thing_name', None)

        if thing_name is None:
            raise ValueError("None 'thing_name'")

        topic = f"$aws/things/{thing_name}/shadow/update"

        payload = {
            "state": {
                "desired": {
                    "dispenser_state": "closed"
                }
            }
        }

        client.publish(topic=topic, qos=1, payload=json.dumps(payload))
        return {
            'statusCode': 200,
        }
    except Exception as e:  
        print(f"Exception: {e}")
