# Use an official Python runtime as a parent image
FROM python:3.10-slim

# Set the working directory in the container
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

RUN apt-get update && apt-get install -y libgomp1

RUN ls lib

RUN ldd lib/pygomoku.so || true  # The `true` is to prevent the build from failing
RUN echo "hello"

# Install any needed packages specified in requirements.txt
RUN pip install --no-cache-dir -r requirements.txt

ENV PYTHONPATH=/app/lib:$PYTHONPATH

# Make port 5000 available to the world outside this container
EXPOSE 5000

# Define environment variable
ENV FLASK_APP=server_gomoku.py

# Run server.py when the container launches
CMD ["flask", "run", "--host=0.0.0.0"]
