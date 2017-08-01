import {connect} from 'react-redux';
import React, {Component,} from 'react';
import PropTypes from 'prop-types';

import Startpage from '../components/Startpage';

class StartpageContainer extends Component {
    render() {
        return <Startpage/>;
    }
}

const mapStateToProps = state => ({
    tiffinfo: state.tiffinfo,
});

const mapDispatchToProps= dispatch => ({ dispatch });

StartpageContainer.propTypes = {
    tiffinfo: PropTypes.object.isRequired,
};

export default connect(mapStateToProps, mapDispatchToProps)(StartpageContainer);
