#!/usr/bin/python
# -*- coding: utf-8 -*-
from __main__ import app, es, model
import os

from flask import request
from werkzeug.utils import secure_filename

@app.route('/api/create', methods = ['POST'])
def create_index():
    try:
        # Create index if here not exist
        model.create()
        return { 'code' : 201, 'message' : "Successfully created!"}
    
    except Exception as e:
        print(e)
        # Else send error
        return { 'code' : 400, 'message' : "Error on creating index!"}
    
    
@app.route('/api/add_file', methods = ['POST'])
def add_document():
    
    # Check if file loaded
    if 'file' not in request.files:
        # Send message to client.
        # Here not loaded file 
        return { 'code' : 400, 'message' : "File not loaded!"}
    
    # Get file from request
    file = request.files['file']
    
    # If file not selected
    # Browser can send empty file
    if file.filename == '':
         return { 'code' : 400, 'message' : "File empty!"}
     
    # Security get filename
    filename = secure_filename(file.filename)
    file_content = file.stream.read().decode()
    
    # Save file in Elasticsearch
    model.add(filename, file_content)
    
    return {
        'code' : 200,
        'message' : 'File Loaded!'
    }

        
        
@app.route('/api/papa', methods = ['POST'])
def papa():
    
    src_filename = request.form['src_filename']
    if src_filename == '__':
        src_filename = 'all'
     # Check if file loaded
    if 'file' not in request.files:
        # Send message to client.
        # Here not loaded file 
        return { 'code' : 400, 'message' : "File not loaded!"}
    
    # Get file from request
    file = request.files['file']
    
    # If file not selected
    # Browser can send empty file
    if file.filename == '':
         return { 'code' : 400, 'message' : "File empty!"}
     
    # Security get filename
    filename = secure_filename(file.filename)
    file_content = file.stream.read().decode()
    
    result = model.papa(file_content, filename, src_filename)
    if 'message' in result:
        return result
    return {
        'src_code' : file_content.split('\n'),
        'dst_code' : list(result['dst_code']),
        'diff' : result['diff'],
        'dst_name' : result['dst_name'],
        'sims' : result['sims']
    }
    
@app.route('/api/field/', methods=['POST'])
def field_list():
    field_name = request.form['name']
    
    data = model.get_field_values(field_name)
    
    return {
        'result' : data
    }