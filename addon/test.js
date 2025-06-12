
// Load the native addon using the 'bindings' module
// This will look for the compiled .node file in various places
const bindings = require('bindings')
const addon = bindings('addon')


// Try the verifyLogin function
const result = addon.verifyLogin('admin', 'admin123')

// Should print: "Hello from C++! You said: This is a test"
console.log(result)
