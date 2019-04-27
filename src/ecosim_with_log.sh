#!/bin/bash
echo "Starting Ecosim..."
./ecosim&
sleep 1;
echo "Starting logger plot"
./logger_plot.py
