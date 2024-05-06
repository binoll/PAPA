import os, json
from elasticsearch_dsl import connections
from dotenv import load_dotenv
from flask import Flask, jsonify, abort, request, make_response, url_for

from models.PAPA import PAPA
from controllers.tokenizers.mpi import tokenizer


# Load .env configuration
dotenv_path = os.path.join(os.path.dirname(__file__), '.env')
if os.path.exists(dotenv_path):
    load_dotenv(dotenv_path)
    
    
es = connections.create_connection(hosts=[os.environ.get("HOST") + ':' + os.environ.get("PORT")], timeout=20)
app = Flask(__name__, static_url_path = "")

with open(os.environ.get("TOKENS_JSON"), 'r', encoding='utf8') as tf:
    model = PAPA(es, tokenizer, tf.read())
    
    
from router import papa, views

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=os.environ.get("PORT_WEB"))