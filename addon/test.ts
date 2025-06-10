import bindings from 'bindings'

const addon = bindings('addon');


// Try the verifyLogin function
const result = addon.verifyLogin('username', 'password');

// Should print: "Hello from C++! You said: This is a test"
console.log(result);
