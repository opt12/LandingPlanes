module.exports = {

    port: '',    //configure the port for the server, default is 3000
    ipv4_address: '', //configure the IPv4 address for the server default is '127.0.0.1'

    // "use npm start with argument to specify database prefix like npm --BASE=Usa start",

    planesDb: `mongodb://localhost:27017/${process.env.npm_config_BASE || 'Test'}Planes`,

    secretKey: '12345',
};
