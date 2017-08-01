const webpack = require('webpack');
const WebpackDevServer = require('webpack-dev-server');
const config = require('./webpack.config');

const serverConfig = {
    publicPath: config.output.publicPath,
    hot: true,
    // historyApiFallback: true,
};

new WebpackDevServer(webpack(config), serverConfig).listen(3001);
