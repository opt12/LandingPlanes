import React from 'react';
import { Route, IndexRoute } from 'react-router';

import App from './components/App';
import StartpageContainer from './containers/StartpageContainer';

export default (
  <Route path="/" component={App}>
    <IndexRoute component={StartpageContainer} />
    {/*<Route path="startpage" component={StartpageContainer} />*/}
  </Route>
);
