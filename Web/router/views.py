#!/usr/bin/python
# -*- coding: utf-8 -*-
from __main__ import app, model

from flask import render_template


@app.route('/', methods = ['GET'])
def main_page():
    return render_template(
        'index.html', 
        subjects=model.get_field_values('subject'),
        work_types=model.get_field_values('work_type'),
        task_nums=model.get_field_values('task_num')
        )
    
    
@app.route('/add', methods = ['GET'])
def add_file_page():
    return render_template('add.html')