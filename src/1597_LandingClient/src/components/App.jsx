import React from 'react';

// import NavBar from '../containers/navbar';

const App = ({children}) => {
    console.log("APP started with these cildren: ", children);
    return (
        <div class="container">
            {/*<NavBar />*/}
            {children}
        </div>
    );
}

export default App;
