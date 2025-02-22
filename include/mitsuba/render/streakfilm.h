#pragma once

#include <mitsuba/core/logger.h>
#include <mitsuba/core/object.h>
#include <mitsuba/core/rfilter.h>
#include <mitsuba/core/vector.h>
#include <mitsuba/mitsuba.h>
#include <mitsuba/render/film.h>
#include <mitsuba/render/fwd.h>
#include <mitsuba/render/sampler.h>

NAMESPACE_BEGIN(mitsuba)

/** \brief Abstract streak film base class - used to store samples
 * generated by \ref TransientIntegrator implementations.
 *
 * To avoid lock-related bottlenecks when rendering with many cores,
 * rendering threads first store results in an "streak image block", which
 * is then committed to the film using the \ref put() method.
 */
template <typename Float, typename Spectrum>
class MTS_EXPORT_RENDER StreakFilm : public Film<Float, Spectrum> {
public:
    MTS_IMPORT_BASE(Film, m_size, m_crop_size, m_crop_offset, m_filter,
                    m_high_quality_edges, bitmap)
    MTS_IMPORT_TYPES(ImageBlock, StreakImageBlock, ReconstructionFilter, Scene,
                     Sensor, Sampler, Medium, TransientSamplingIntegrator)

    bool should_auto_detect_bins() const { return m_auto_detect_bins; }

    /// Set start_opl and bin_width_opl based on preliminary testing
    void auto_detect_bins(Scene *scene, Sensor *sensor);

    /// Merge an image block into the film. This methods should be thread-safe.
    virtual void put(const StreakImageBlock *block) = 0;

    /// Return the bitmap object storing the developed contents of the film
    /// corresponding to the i-th slice x-t
    virtual ref<Bitmap> bitmap(int slice, bool raw) = 0;

    // =============================================================
    //! @{ \name Accessor functions
    // =============================================================

    size_t num_bins() const { return m_num_bins; }

    float bin_width_opl() const { return m_bin_width_opl; }

    float start_opl() const { return m_start_opl; }

    Float end_opl() const { return start_opl() + num_bins() * bin_width_opl(); }

    const ReconstructionFilter *time_reconstruction_filter() const {
        return m_time_filter.get();
    }

    //! @}
    // =============================================================

    virtual std::string to_string() const override;

    MTS_DECLARE_CLASS()
protected:
    /// Create a film
    StreakFilm(const Properties &props);

    /// Virtual destructor
    virtual ~StreakFilm();

protected:
    uint32_t m_num_bins;
    float m_bin_width_opl;
    float m_start_opl;
    bool m_auto_detect_bins;
    /// NOTE(diego): this is not implemented (yet)
    ref<ReconstructionFilter> m_time_filter;
};

MTS_EXTERN_CLASS_RENDER(StreakFilm)
NAMESPACE_END(mitsuba)
