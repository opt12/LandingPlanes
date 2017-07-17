import { connect } from 'react-redux';

import NavBarForm from '../components/Navbar';

const mapStateToProps = state => ({
    fileInfo: state.fileinfo,
});

export default connect(mapStateToProps)(NavBarForm);
