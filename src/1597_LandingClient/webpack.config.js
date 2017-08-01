const path = require('path');
const webpack = require('webpack');

module.exports = {
  devtool: 'eval-source-maps',
  entry: [
    'react-hot-loader/patch',
    'webpack-dev-server/client?http://localhost:3001',
    'webpack/hot/only-dev-server',
    './src/main',
  ],
  output: {
    path: path.join(__dirname, 'dist'),
    filename: 'bundle.js',
    publicPath: '/static/',
  },
  plugins: [
    new webpack.HotModuleReplacementPlugin(),
  ],
  module: {
    rules: [
      {
        test: /\.(js|jsx)?$/,
        use: {
          loader: 'babel-loader',
        },
        include: path.join(__dirname, 'src'),
      },
      {
        test: /\.css$/,
        use: [
                    { loader: 'style-loader' },
                    { loader: 'css-loader' },
        ],
      },
      {
        test: /\.useable\.css$/,
        use: [
          {
            loader: 'style-loader',
            options: {
              useable: true,
            },
          },
                    { loader: 'css-loader' },
        ],
      },
    ],
  },
  devServer: {
    overlay: true,
    historyApiFallback: true,
  },
  resolve: {
    extensions: ['.js', '.jsx'],
  },
};
