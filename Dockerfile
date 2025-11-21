# Use a base image with Node.js and build tools
FROM node:18-bullseye

# Install C/C++ build tools
RUN apt-get update && apt-get install -y build-essential && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy all the files from the project
COPY . .

# Build the C application
RUN make clean
RUN make

# Install npm dependencies for the web interface
WORKDIR /app/web
RUN npm install
WORKDIR /app

# Expose the port the web server runs on
EXPOSE 3000

# Define the command to run the application
# This will start the node server. The C application is a kernel,
# so it's not something that is "run" continuously in the same way.
# The web interface will likely interact with it.
CMD ["node", "web/server.js"]
